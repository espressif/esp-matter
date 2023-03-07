#!/usr/bin/perl
#
# Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#
use READELF;

my $bind_lut = { 0 => 'local', 1 => 'global', 2 => 'weak', 13 => 'lo_proc', 15 => 'hi_proc' };
my $type_lut = { 0 => 'no type', 1 => 'object', 2 => 'function', 3 => 'section', 4 => 'file', 13 => 'lo_proc', 15 => 'hi_proc'};


# read libraries *.a and generate libinstaller.c to call init and install patch entries

main();

sub main
{
	my $overlay_wrappers = [];
	my $unique = {};

	my $outfile = pop @ARGV;
	my @path_array = split /\/|\\/, $outfile;
	my ($outdir,$path_sep, $mkdir_command);
	my $patch_entry_count = 0;

	if($^O =~ /MSWin32/) {
		$path_sep = "\\";
		$mkdir_command = "md";
	}
	else {
		$path_sep = "/";
		$mkdir_command = "mkdir -p";
	}
	$outdir = join $path_sep, @path_array[0..$#path_array-1];
	$outfile = join $path_sep, @path_array;
	my $result = `$mkdir_command $outdir` unless -d $outdir;

	open(my $OUT, ">", $outfile) || die "ERROR: Cannot open $outfile, $!";

	print $OUT "// $0\n";
	print $OUT "// args: @ARGV\n";
	print $OUT "// output: $outfile\n";
	print $OUT "#include <stdint.h>\n";
	print $OUT "typedef struct tag_PATCH_TABLE_ENTRY_t {\n";
	print $OUT "\tuint32_t breakout;\n";
	print $OUT "\tuint32_t replacement;\n";
	print $OUT "} PATCH_TABLE_ENTRY_t;\n";
	print $OUT "void patch_autoInstall(uint32_t old_address, uint32_t new_address);\n";
	print $OUT "void patch_autoReplace(uint32_t breakout_address, uint32_t replacement);\n";
	print $OUT "void patch_autoReplaceData(uint32_t breakout_address, uint32_t replacement);\n";
	print $OUT "void install_libs(void) __attribute__((section(\".spar_setup\")));\n";
	print $OUT "\nvoid install_libs(void)\n{\n";

	foreach my $lib (@ARGV) {
		my $sections = [];
		my $stringtable = {};
		my $sym_str_tbl = {};
		my $symbol_entries = [];
		parse_archive($lib, $sections, $stringtable, $sym_str_tbl, $symbol_entries, 0x20);
		my $init = $lib;
		#print "process $lib\n";
		# extract lib name -- works for linux-style path, not DOS
		$init =~ s!^(?:.*/)?(.+?)(?:\.[^.]*)?$!$1!;
		$init .= "_init";
		# look for library symbols indicating patch entries and init calls
		foreach my $sym (@{$symbol_entries}) {
			my $name = $sym->{name};
			next if !defined $name;
			#print "$name\n";
			if ($name =~ /auto_install_(.*)/) {
				my $fn = "auto_install_$1";
				next if defined $unique->{$fn};
				$unique->{$fn}++;
				print $OUT "\t{\n\t\textern void $fn(void);\n\t\t$fn();\n\t}\n";
			}
			elsif ($name =~ /$init(.*)/)
			{
				next if defined $unique->{$init};
				$unique->{$init}++;
				print $OUT "\t{\n\t\textern void $init(void);\n\t\t$init();\n\t}\n";
			}
			elsif ($name =~ /^install_patch_/) {
				next if defined $unique->{$name};
				$unique->{$name}++;
				print $OUT "\t{\n\t\textern void $name(void);\n\t\t$name();\n\t}\n";
				next if $name =~ /hookInit$/;
				$patch_entry_count++;
			}
			elsif ($name =~ /(__patch_.*)/) {
				next if defined $unique->{$name};
				$unique->{$name}++;
				handle_patch_entry($1, $OUT);
				$patch_entry_count++;
			}
		}
		# look in library *.o for overlays
		foreach my $ar_section (@{$sections}) {
			my $overlay_info = {};
			$ar_section->{'name'} = substr($ar_section->{header}->{ar_name},0,16);
			next unless $ar_section->{name} =~ /^_\d_/;
			$overlay_info->{'ar_section'} = $ar_section;
			$overlay_info->{'functions'} = [];
			$overlay_info->{'symbols'} = {};
			$overlay_info->{'lib'} = $lib;
			#printf "archive %s\n", substr($ar_section->{header}->{ar_name},0,16);
			foreach my $section (@{$ar_section->{section_headers}}) {
				next if 0 == $section->{sh_size};
				next if $section->{sh_type} & 0xfffffffc;
			#	printf "%s flag %x info %x type %x size %x offset %x file offset %x\n", $section->{name}, $section->{sh_flags}, $section->{sh_info}, $section->{sh_type},
			#							$section->{sh_size}, $section->{sh_offset}, $section->{header_file_offset};
			}
			foreach my $sym (@{$ar_section->{symbol_entries}}) {
				next if ($sym->{st_info} >> 4) != 1;
				next if ($sym->{st_info} & 0xf) != 2;
				$overlay_info->{symbols}->{$sym->{name}} = $sym;
			#	printf "# symbol %s has value %x size %x info type=%s binding=%s\n", $sym->{name}, $sym->{st_value}, $sym->{st_size},
			#						$type_lut->{(($sym->{st_info} >> 4)&0xf)}, $bind_lut->{($sym->{st_info} & 0xf)};
				push @{$overlay_info->{functions}}, $sym;
			}
			push @{$overlay_wrappers}, $overlay_info;
		}
	}
	print $OUT "}\n";
	print $OUT "#define PATCH_ENTRIES_WITH_LIBRARIES ($patch_entry_count + CY_PATCH_ENTRIES_BASE)\n";
	print $OUT "#if PATCH_ENTRIES_WITH_LIBRARIES > NUM_PATCH_ENTRIES\n";
	print $OUT "#error Too many patch entries for device, after adding libraries\n";
	print $OUT "#endif\n";
	close $OUT;

	return if scalar(@{$overlay_wrappers}) == 0;

	# process overlays
	generate_overlay_ld_snip($outdir, $overlay_wrappers);

	foreach my $wrapper (@{$overlay_wrappers}) {
		output_trampoline_wrapper($outdir, $wrapper);
	}
}

sub generate_overlay_ld_snip
{
	my ($outdir, $wrappers) = @_;
	my $count = 0;
	my $filename = "$outdir/overlays.ld";

	open(my $LD, ">", $filename) || die "ERROR: Cannot open $filename, $!";

	# Generate the header.
	print $LD "  OVERLAY : NOCROSSREFS\n  {\n";

	# Now print each section/object file with the .text section.
	foreach my $wrapper (@{$wrappers}) {
		my $oname;
		my $name = $wrapper->{ar_section}->{name};
		$name =~ s/\/$//;
		$oname = $name;
		$name =~ s/\.o$//;
		print $LD '    .' . $name . "{ " . $name . "_start = \. ; " . "*$oname\(.text.*) ;" . $name . "_end = \. ; " . "}\n";
		$post .= "    PROVIDE(_section_idx_$count = $count);\n";
		$post .= "    PROVIDE(" . $name . "_size = SIZEOF(\." . $name . "));\n";
		$count++;
	}

	$post .= "    PROVIDE(spar_num_overlays = $count);\n";
	# Generate the footer of the overlay placement.
	print $LD "  } >ram\n\n";
	print $LD "\n$post\n\n";
	close $LD;

	open(my $LD_INC, ">", "$outdir/wrap_flags.inc") || die "ERROR: Cannot open $filename, $!";
	foreach my $wrapper (@{$wrappers}) {
		foreach my $function (@{$wrapper->{functions}}) {
			print $LD_INC "-Wl,--wrap=$function->{name} ";
		}
	}
	close $LD_INC;
}

sub output_trampoline_wrapper
{
	my ($outdir, $wrapper) = @_;
	my $filenum;
	my $basename;
	my $symbolsRefs;
	my $fcount = 0;
	my $filename = $wrapper->{ar_section}->{name};
	if($filename =~ /_(\d)_(.*).o/)	{
		$filenum = $1;
		$basename = uc $2;
	}
	else {
		die "File $filename does not begin with _[:digit:]_\n";
	}
	$filename =~ s/\/$//;
	$filename =~ s/\.o$/\.S/;
	open(my $TRAMP, ">", "$outdir/trampoline_$filename") || die "ERROR: Cannot open $filename, $!";
	print $TRAMP "    .syntax unified\n    .cpu cortex-m3\n    .thumb\n    .extern spar_ovm_load_and_exec\n\n    .align 2\n";
	print $TRAMP "    .global CALL_OVM_FROM_$basename\n    .thumb_func\n    .type CALL_OVM_FROM_$basename, \%function\n\n";
	print $TRAMP "CALL_OVM_FROM_$basename:\n";
	print $TRAMP "    movs r0, \#$filenum\n";
	print $TRAMP "    b spar_ovm_load_and_exec\n";
	print $TRAMP "    .size CALL_OVM_FROM_$basename, .-CALL_OVM_FROM_$basename\n\n";

	foreach my $function (@{$wrapper->{functions}}) {
		my $name = $function->{name};
		#printf "# symbol %s has value %x size %x info type=%s binding=%s\n", $function->{name}, $function->{st_value}, $function->{st_size},
		#							$type_lut->{$function->{st_info} >> 4}, $bind_lut->{$function->{st_info} & 0xf};
		print $TRAMP "    .align 1\n";
		print $TRAMP "    .global __wrap_$name\n    .thumb_func\n    .type __wrap_$name, \%function\n\n";
		print $TRAMP "__wrap_$name:\n";
		print $TRAMP "    push {r0, r1, r2, r3, r4, lr}\n";
		print $TRAMP "    ldr r4, .L$fcount\n";
		$symbolsRefs .= "    .extern  __real_$name\n    .L$fcount: .word __real_$name\n";
		print $TRAMP "    b CALL_OVM_FROM_$basename\n";
		print $TRAMP "    .size __wrap_$name, .-__wrap_$name\n\n";
		$fcount++;
	}
	print $TRAMP "$symbolsRefs\n\n";
	close $TRAMP;
}

sub handle_patch_entry
{
	my ($patch_symbol, $fh) = @_;
	my $patch_install_fn;

	print $fh "\t{\n";
	print $fh "\t\textern PATCH_TABLE_ENTRY_t $patch_symbol;\n";
	if($patch_symbol =~ /^__patch__/) {
		$patch_install_fn = "patch_autoInstall";
	}
	elsif($patch_symbol =~ /^__patch_addin__/) {
		$patch_install_fn = "patch_autoInstall";
	}
	elsif($patch_symbol =~ /^__patch_rpl_/) {
		$patch_install_fn = "patch_autoReplace";
	}
	elsif($patch_symbol =~ /^__patch_data__/) {
		$patch_install_fn = "patch_autoReplaceData";
	}
	print $fh "\t\t$patch_install_fn\($patch_symbol\.breakout & 0x00ffffff, $patch_symbol\.replacement\);\n";
	print $fh "\t}\n";
}
