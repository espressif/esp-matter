#!/bin/perl
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

# read all elf sections into $section_headers array of $section hashes
# provide $section->{name}, $section->{data} along with other elf section data

####################################################################################################
### parse elf file(s) for sectiona data and symbols
sub parse_elf
{
	my ($filename, $section_headers, $stringtable, $sym_str_tbl, $symbol_entries, $extra) = @_;
	my $buffer = ();
	my $bytes_read;
	my $ELFFILE;
	my @elf_ident;
	my %elf_hdr = ();

	$extra = 0 if !defined $extra;

	open( $ELFFILE, "<", $filename) || die "ERROR: Cannot open $filename, $!";
	binmode($ELFFILE);

	read_opened_elf($ELFFILE, $filename, $section_headers, $stringtable, $sym_str_tbl, $symbol_entries, $extra);

	close $ELFFILE;
	if($extra & 0x10) {
		foreach my $section (@{$section_headers}) {
			printf "%02d section %-28s type %02x flags %02x addr %08x size %08x info %02x data len %08x\n",
					$section->{index}, $section->{name}, $section->{sh_type},
					$section->{sh_flags},$section->{sh_addr}, $section->{sh_size}, $section->{sh_link},
					$section->{sh_info}, length($section->{data});
		}
	}

	decompress_sections($section_headers, $symbol_entries) if ($extra & 0x2);
}

sub read_opened_elf
{
	my ($ELFFILE, $filename, $section_headers, $stringtable, $sym_str_tbl, $symbol_entries, $extra) = @_;
	my $buffer = ();
	my $bytes_read;
	my @elf_ident;
	my %elf_hdr = ();
	my $section_stringtable = {};
	my $elf_start = tell($ELFFILE);

	# check magic number of elf file
#	print "\nParsing $filename...\n";
#	print "reading elf header\n";
	read($ELFFILE, $buffer, 16);
	@elf_ident = unpack("C[16]",$buffer);
	if((0x7f != $elf_ident[0]) || ('E' ne chr($elf_ident[1])) || ('L' ne chr($elf_ident[2])) || ('F' ne chr($elf_ident[3]))) {
		die "elf magic number not found in $filename\n";
	}

	read($ELFFILE, $buffer, 36);

	(	$elf_hdr{"e_type"},
		$elf_hdr{"e_machine"},
		$elf_hdr{"e_version"},
		$elf_hdr{"e_entry"},
		$elf_hdr{"e_phoff"},
		$elf_hdr{"e_shoff"},
		$elf_hdr{"e_flags"},
		$elf_hdr{"e_ehsize"},
		$elf_hdr{"e_phentsize"},
		$elf_hdr{"e_phnum"},
		$elf_hdr{"e_shentsize"},
		$elf_hdr{"e_shnum"},
		$elf_hdr{"e_shstrndx"}) = unpack("SSLLLLLSSSSSS",$buffer);
	if($elf_hdr{e_type} == 1) {
		$elf_hdr{e_shoff} += $elf_start;
	}
	if($elf_hdr{e_shoff}) {
		read_section_hdrs($ELFFILE, \%elf_hdr, $section_headers, $stringtable, $section_stringtable, $elf_start);
	}
	return if $extra & 0x20;
	my $index = -1;
	#printf "elf has %d section headers\n", scalar(@{$section_headers});
	foreach my $section (@{$section_headers}) {
		$index++;
		#print "start section $index, $extra\n";
		if(0 == ($extra & 0x1)) {
			next if !defined $section->{sh_name};
			#next if !defined $stringtable->{$section->{sh_name}};
			next if $section_stringtable->{$section->{sh_name}} =~ /^\.debug/;
			next if $section_stringtable->{$section->{sh_name}} =~ /^\.note/;
			next if $section->{sh_size} == 0;
		}
		if($section->{sh_offset} > 0x10000000) {
			printf "section $index offset 0x%x, %s\n", $section->{sh_offset}, $section_stringtable->{$section->{sh_name}};
			next;
		}
		if($section->{sh_size} > 0x100000) {
			printf "section $index size 0x%x, %s\n", $section->{sh_s}, $section_stringtable->{$section->{sh_name}};
			next;
		}
		seek $ELFFILE, $section->{sh_offset}, 0;
		read($ELFFILE, $buffer, $section->{sh_size});
		#printf "  read section $index len %x  from %x\n", length($buffer),  $section->{sh_offset};
		die "section $index read incorrectly\n" if length($buffer) != $section->{sh_size};
		$section->{"data"} = $buffer;
		if(%{$section_stringtable}) {
			$section->{"name"} = $section_stringtable->{$section->{sh_name}};
		}
		# sometimes section name is a subset of another string in table
		if(!defined $section->{name}) {
			my $containing_str;
			# printf "look for offset 0x%x\n",$section->{sh_name};
			my @indexes = sort {$a <=> $b} keys(%{$section_stringtable});
			foreach my $str_offset (@indexes) {
				last if $str_offset > $section->{sh_name};
				$containing_str = $str_offset;
			}
			if(defined $containing_str && length($section_stringtable->{$containing_str}) > $section->{sh_name} - $containing_str) {
				$section->{"name"} = substr($section_stringtable->{$containing_str}, $section->{sh_name} - $containing_str);
			}
		}
		if($section->{sh_type} == 2) {
			read_symbols($ELFFILE, $section, $section_headers, $sym_str_tbl, $symbol_entries);
		}
	}
	if($extra & 0x10) {
		foreach my $section (@{$section_headers}) {
			printf "%02d section %-28s sh_name %x type %02x flags %02x addr %08x size %08x link %x info %02x data len %08x file offset %x\n",
					$section->{index}, $section->{name}, $section->{sh_name}, $section->{sh_type},
					$section->{sh_flags},$section->{sh_addr}, $section->{sh_size}, $section->{sh_link},
					$section->{sh_info}, length($section->{data}), $section->{header_file_offset};
		}
	}
}

sub parse_archive
{
	my ($filename, $section_headers_array, $stringtable1, $sym_str_tbl1, $symbol_entries, $extra) = @_;
#	my ($filename, $section_headers, $stringtable, $sym_str_tbl, $symbol_entries, $extra) = @_;
	my $buffer = ();

	$extra = 0 if !defined $extra;
	#print "opening $filename\n";

	open(my $AR, "<", $filename) || die "ERROR: Cannot open $filename, $!";
	binmode($AR);
	die "Could not read $filename magic\n" if 8 != read($AR, $buffer, 8);
	my ($magic) = unpack "C*", $buffer;
	die "Invalid file magic string\n" if !$magic eq '!<arch>\x0a';
	while(!eof($AR)) {
		my $header = {};
		$header->{'tell'} = tell $AR;
		#printf "start read at offset 0x%x\n", $header->{'tell'};
		die "Could not read $filename file header\n" if 60 != read($AR, $buffer, 60);
		(	$header->{'ar_name'},
			$header->{'ar_date'},
			$header->{'ar_uid'},
			$header->{'ar_gid'},
			$header->{'ar_mode'},
			$header->{'ar_size'},
			$header->{'ar_fmag'}) = unpack "A[16]A[12]A[6]A[6]A[8]A[10]C[2]", $buffer;
		die "Invalid file header\n" if !$header->{ar_fmag} =~ /^\x60\x0a$/;
		#printf "ar %s at offset 0x%x size is %d (0x%x)\n", substr($header->{ar_name},0,16), $header->{tell}, $header->{ar_size}, $header->{ar_size};
		if(substr($header->{'ar_name'},0,2) eq "/") {
			#print "got archive symbol table\n";
			die "Problem reading $filename\n" if 4 != read($AR, $buffer, 4);
			my ($num_sym) = unpack "N", $buffer;
			die "Problem reading $filename\n" if (4*$num_sym) != read($AR, $buffer, 4*$num_sym);
			#print "Reading $num_sym symbols\n";
			my @offsets = unpack "N[$num_sym]", $buffer;
			my $sym_len = $header->{ar_size} - (4 * ($num_sym+1));
			#printf "reading symbol names len %d\n", $sym_len;
			die "Problem reading $filename symbols\n" if $sym_len != read($AR, $buffer, $sym_len);
			my @syms = split "\x00", $buffer;
			die "symbols and offsets mismatch\n" if scalar(@offsets) != scalar(@syms);
			while(defined(my $addr = shift @offsets)) {
				my $s = {};
				$s->{'st_value'} = $addr;
				$s->{'name'} = shift @syms;
				push @{$symbol_entries}, $s;
			}
		}
		elsif(substr($header->{'ar_name'},0,2) eq "//") {
			#print "got archive object list\n";
			my $sym_len = $header->{ar_size};
			#printf "reading symbol names len %d\n", $sym_len;
			die "Problem reading $filename symbols\n" if $sym_len != read($AR, $buffer, $sym_len);
			my @syms = split "\x2f\x0a", $buffer;
			#foreach my $sym (@syms) {
			#	print "$sym\n";
			#}
		}
		else {
			my $section_headers = [];
			my $ar_section = {};
			$ar_section->{'header'} = $header;
			$ar_section->{'stringtable'} = {};
			$ar_section->{'sym_str_tbl'} ={};
			$ar_section->{'section_headers'} = [];
			$ar_section->{'symbol_entries'} = [];
			my $elf_start = tell($AR);
			#printf "about to read elf archive at 0x%x\n", $elf_start;
			read_opened_elf($AR, $filename, $ar_section->{section_headers}, $ar_section->{stringtable}, $ar_section->{sym_str_tbl}, $ar_section->{symbol_entries}, 0);
			push @{$symbol_entries}, @{$ar_section->{symbol_entries}};
			push @{$section_headers_array}, $ar_section;
			seek $AR, $elf_start + $header->{ar_size}, 0;
			#printf "after reading elf archive at 0x%x\n", tell($AR);
		}
		if(tell($AR) & 1) {
			seek $AR, 1, 1;
		}
	}

	close($AR);
}

sub dump_buffer
{
	my ($buffer) = @_;
	my @bytes = unpack "C*", $buffer;
	foreach my $byte (@bytes) {
		printf "%02x ", $byte;
	}
	print "\n";
}

sub read_symbols
{
	my ($file, $section, $sections, $sym_str_tbl, $symbol_entries) = @_;
	my $sym_str_tbl_section;
	my $num_entries;
	my $buffer;

	if($section->{sh_entsize} == 0){
		die "invalid symbol table\n";
	}

	$num_entries = $section->{sh_size} / $section->{sh_entsize};
	#printf "symbol table has %d entries size %08x div by entry %02x\n", $num_entries,
	#			$section->{sh_size}, $section->{sh_entsize};

	if( ($num_entries * 12) > $section->{sh_size} + 1) {
		die "invalid symbol table size\n";
	}
	$section->{"symbols"} = $symbol_entries;

	# printf "symbol link index 0x%02x\n", $section->{sh_link};
	foreach my $hdr_ref (@{$sections}) {
		if( (defined $hdr_ref->{e_shstrndx}) && ($hdr_ref->{e_shstrndx} != 0) && ($hdr_ref->{e_shstrndx} < $hdr_ref->{e_shnum})) {
			#printf "string table section index 0x%02x\n", $hdr_ref->{e_shstrndx};
			if($section->{sh_link} == $hdr_ref->{e_shstrndx}) {
				# print "symbol string table is same\n";
				$sym_str_tbl_section = $hdr_ref;
				last;
			}
		}
		if($section->{sh_link} == $hdr_ref->{index}) {
			# printf "found sym strings at 0x%02x\n", $hdr_ref->{index};
			$sym_str_tbl_section = $hdr_ref;
			last;
		}
	}
	#printf "reading symbol string table, file offset %x\n", $sym_str_tbl_section->{sh_offset};
	read_string_table($file, $sym_str_tbl_section, $sym_str_tbl);

	seek $file, $section->{sh_offset}, 0;
	for(my $i = 0; $i < $num_entries; $i++) {
		read($file, $buffer, 16);
		my $symbol = ();
		(	$symbol->{"st_name"},
			$symbol->{"st_value"},
			$symbol->{"st_size"},
			$symbol->{"st_info"},
			$symbol->{"st_other"},
			$symbol->{"st_shndx"}) = unpack("LLLCCS",$buffer);
		$symbol->{'name'} = read_string_from_table($sym_str_tbl, $symbol->{st_name});
		#print "read symbol $symbol->{name}\n";
		push @{$symbol_entries}, $symbol;
	}
}

sub read_string_from_table
{
	my ($table, $index) = @_;
	my $str = $table->{$index};
	if(!defined $str) {
		my $containing_str;
		# printf "look for offset 0x%x\n",$section->{sh_name};
		my @indexes = sort {$a <=> $b} keys(%{$table});
		foreach my $str_offset (@indexes) {
			last if $str_offset > $index;
			$containing_str = $str_offset;
		}
		if(defined $containing_str && length($table->{$containing_str}) > $index - $containing_str) {
			$str = substr($table->{$containing_str}, $index - $containing_str);
		}
	}
	return $str;
}

sub read_section_hdrs
{
	my ($file, $hdr_ref, $section_headers, $stringtable, $section_stringtable, $elf_start) = @_;
	my $buffer;

	#printf "read section at 0x%x\n", $hdr_ref->{e_shoff};
	seek $file, $hdr_ref->{e_shoff}, 0;
	for(my $i = 0; $i < $hdr_ref->{e_shnum}; $i++) {
		my $section_hdr = {};
		$section_hdr->{"header_file_offset"} = tell($file);
		read($file, $buffer, $hdr_ref->{e_shentsize});
		(	$section_hdr->{"sh_name"},
			$section_hdr->{"sh_type"},
			$section_hdr->{"sh_flags"},
			$section_hdr->{"sh_addr"},
			$section_hdr->{"sh_offset"},
			$section_hdr->{"sh_size"},
			$section_hdr->{"sh_link"},
			$section_hdr->{"sh_info"},
			$section_hdr->{"sh_addralign"},
			$section_hdr->{"sh_entsize"}) = unpack("L[10]",$buffer);
			$section_hdr->{"index"} = $i;
			# add offset if elf is in archive section
			$section_hdr->{sh_offset} += $elf_start;
		push @{$section_headers}, $section_hdr;
	}
	if( ($hdr_ref->{e_shstrndx} != 0) && ($hdr_ref->{e_shstrndx} < $hdr_ref->{e_shnum})) {
		# printf "section name string table section index 0x%04x\n", $hdr_ref->{e_shstrndx};
		read_string_table($file, $section_headers->[$hdr_ref->{e_shstrndx}], $section_stringtable);
	}
	else {
		die "no string table entry\n";
	}
}

sub read_string_table
{
	my ($file, $section, $table) = @_;
	my $buffer;
	my $offset = 0;

	if(0 == $section->{sh_offset}) {
		$table->{0} = "";
		#print "no string table in this archive\n";
		return;
	}

	#printf "read string table section %d at 0x%x\n", $section->{index}, $section->{sh_offset};
	seek $file, $section->{sh_offset}, 0;
	if($section->{sh_size} != read($file, $buffer, $section->{sh_size})) {
		#warn "could not read string table\n";
		return;
	}
	my @strings = split /\000/, $buffer;
	foreach my $string (@strings) {
		$table->{$offset} = $string;
		$offset += length($string) + 1;
	}
}

sub find_symbol
{
    my ($symbol_entries, $symbol_name) = @_;
    foreach my $sym (@{$symbol_entries}) {
        if($sym->{name} eq $symbol_name) {
            #print "found \"$symbol_name\" at ", sprintf "0x%06x\n", $sym->{st_value};
            return $sym;
        }
    }
    #print "Could not find symbol \"$symbol_name\"\n";
    return undef;
}

sub find_symbol_with_address
{
    my ($symbol_entries, $symbol_addr, $symbol_name) = @_;
    foreach my $sym (@{$symbol_entries}) {
        next if($sym->{st_value} != $symbol_addr);
        next if !($sym->{name} =~ /$symbol_name/);
      #  print "found \"$sym->{name}\" at ", sprintf "0x%06x\n", $symbol_addr;
        return $sym;
    }
    return undef;
}

sub find_symbol_at_address
{
    my ($symbol_entries, $addr, $regex) = @_;
    foreach my $sym (@{$symbol_entries}) {
        if($sym->{st_value} eq $addr) {
		if(defined $regex) {
			next if ($sym->{name} =~ /cfg_glb_var/);
			return $sym if $sym->{name} =~ /$regex/;
			next;
		}
            #print "found \"$sym->{name}\" at ", sprintf "0x%06x\n", $sym->{st_value};
		return $sym;
        }
    }
    #printf "Could not find symbol at %x\n", $addr;
    return undef;
}

sub find_function_containing_address
{
    my ($symbol_entries, $symbol_addr) = @_;
    my $fn = undef;

    foreach my $sym (@{$symbol_entries}) {
	my $addr = $sym->{st_value};
        next if($symbol_addr < $addr);
        next if($symbol_addr > ($addr + $sym->{st_size}) );
        next if(($sym->{st_info} & 0xf) != 2 );
        $fn = $sym;
    }
    if(!defined($fn)) {
    #	printf "could not find function or var containing 0x%08x\n", $symbol_addr;
    }
    return $fn;
}

sub decompress_sections
{
	my ($section_headers, $symbol_entries) = @_;

	# find and decompress any compressed sections
	my $table_base = find_symbol($symbol_entries, "Region\$\$Table\$\$Base");
	die "could not find Region\$\$Table\$\$Base\n" if !defined $table_base;
	my $table_limit = find_symbol($symbol_entries, "Region\$\$Table\$\$Limit");
	die "could not find Region\$\$Table\$\$Base\n" if !defined $table_base;
	my $table_section = $section_headers->[$table_base->{st_shndx}];
	my $table_offset = $table_base->{st_value} - $table_section->{sh_addr};
	my $table_len = $table_limit->{st_value} - $table_base->{st_value};

	#printf "region table found in section $table_section->{name} (%x, len %x) at %x (offset %x len %x)\n",
	#	$table_section->{sh_addr}, $table_section->{sh_size},
	#	$table_base->{st_value}, $table_offset, $table_len;
	my @table;
	for(my $i = 0; $i < ($table_len/(4*4)); $i++) {
		my $entry = {};
		(	$entry->{'src'},
			$entry->{'dst'},
			$entry->{'len'},
			$entry->{'fn'}) = unpack "L*", substr($table_section->{data}, $table_offset, 4*4);
		$table_offset += 4*4;
		#printf "table %08X %08X %08X %08X\n", $entry->{'src'}, $entry->{'dst'}, $entry->{'len'},$entry->{'fn'};
		push @table, $entry;
	}
	# load up decompression commands
	my @decomp_table;
	my @decompression_types = qw(__decompress1 __decompress0 __decompress2);
	foreach my $entry (@table) {
		my $sym = find_symbol_at_address($symbol_entries, $entry->{'fn'} | 1);
		#print "found $sym->{name}\n";
		if($sym->{name} =~ /__decompress/) {
			my $decomp_type;
			foreach my $decomp (@decompression_types) {
				my $sym1 = find_symbol($symbol_entries, $decomp);
				if(defined $sym1) {
					$decomp_type = $decomp;
					last;
				}
			}
			die "Decompression type is not supported\n" if((!defined $decomp_type) || ($decomp_type ne '__decompress1'));
			#printf "  $decomp_type %x bytes from %x to %x\n",
			#	$entry->{'len'}, $entry->{'src'}, $entry->{'dst'};
			# find dest section
			foreach my $sect (@{$section_headers}) {
				next if $sect->{sh_size} == 0;
				next if !($sect->{sh_type} & 1);
				next if $sect->{sh_addr} > $entry->{'dst'};
				next if ($sect->{sh_addr} + $sect->{sh_size}) <= $entry->{'dst'};
				#printf "decompress $sect->{name} len %x\n", length($sect->{data});
				$sect->{data} = decompress1($sect->{data}, $entry->{len});
				$sect->{sh_size} = length($sect->{data});
				#printf "now $sect->{name} has decompressed data len %x\n", length($sect->{data});
				last;
			}
		}
	}
}

sub decompress1
{
	my ($data, $len) = @_;
	my ($len_nz, $len_z, $pt_dst, $byte, $out);
	my $dst = 0;
	my $src = 0;
	my $end = $dst + $len;
	my $loop_count = 0;

	#printf "decompress len %08x\n", $len;

	while($dst < $end) {
		# get hdr
		die if $src >= length $data;
		(my $hdr) = unpack("C", substr($data, $src++, 1));
		last if !defined $hdr;
		#printf "hdr = %02x from %08x\n", $hdr, $src-1;
		# if hdr & 7 == 0, get count1
		my $cnt_nz = $hdr & 7;
		if($cnt_nz == 0) {
			($cnt_nz) = unpack("C", substr($data, $src++, 1));
		}
	#	printf "cnt_nz = %02x\n", $cnt_nz;
		my $cnt_z = $hdr >> 4;
		# if header ms nib == 0, read count2
		if($cnt_z == 0) {
			($cnt_z) =  unpack("C", substr($data, $src++, 1));
		}
	#	printf "cnt_z = %02x\n", $cnt_z;
		# transfer count1 from src to dst
		while(--$cnt_nz > 0) {
			($byte) = unpack("C", substr($data, $src++, 1));
			$out .= pack("C", $byte);
	#		printf "transfer %02x from %08x to %08x\n", $byte, $src-1, $dst;
			$dst++;
		}
		# if header & 8 != 0, repeat recent pattern
		if($hdr & 8) {
	#		print "hdr & 8 != 0\n";
			# read another count3
			(my $pt_rep) = unpack("C", substr($data, $src++, 1));
			# point back to previous pattern to repeat
			$cnt_z += 2;
	#		printf "repeat pattern from dest - %02x, len %02x\n", $pt_rep, $cnt_z;
			while($cnt_z--) {
				my $b = substr($out, -$pt_rep, 1);
				$out .= $b;
				($b) = unpack("C", $b);
	#			printf "move from dst %02x to %08x\n", $b, $dst;
				$dst++;
			}
		}
		else {
	#		print "hdr & 8 == 0\n";
			while($cnt_z--) {
				$out .= pack("C", 0);
	#			printf "set 00 to %08x\n", $dst;
				$dst++;
			}
		}
		$loop_count++;
	}
	#printf "returning %x bytes decompressed\n", length($out);
	return $out;
}


1;
