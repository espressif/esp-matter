""" Convenience tool to convert intelhex files to binary blobs """
import sys

import click
from intelhex import IntelHex


class BasedIntType(click.ParamType):
    """ click argument type to use a number with specified base """
    name = 'integer'

    def convert(self, value, param, ctx):
        if isinstance(value, int):
            return value
        try:
            return int(value, 0)
        except ValueError:
            self.fail(f"{repr(value)} is not a valid integer (must be decimal, or start with 0x, 0b or 0o", param, ctx)


BASED_INT = BasedIntType()


def _read_binary(input_file, address):
    intermediate = IntelHex()
    intermediate.loadbin(input_file, offset=address)
    if address:
        intermediate.start_addr = {'EIP': address}
    return intermediate


def _read_ihex(input_file, address):
    intermediate = IntelHex(input_file)

    if address:
        relocated = IntelHex()

        for segment_start, segment_end in intermediate.segments():
            relocated_segment_start = segment_start + address
            relocated.puts(relocated_segment_start, intermediate[segment_start:segment_end].tobinstr())

        if intermediate.start_addr:
            relocated.start_addr = {}
            for key in intermediate.start_addr.keys():
                relocated.start_addr[key] = intermediate.start_addr[key] + address

        intermediate = relocated

    return intermediate


def _read_hexstring(input_file, address):
    with open(input_file, 'r', encoding='utf-8') as f:
        hex_string = f.read().replace('\r', '').replace('\n', '')
    intermediate = IntelHex()
    intermediate.puts(address, ''.join([chr(int(hex_string[i:i + 2], 16)) for i in range(0, len(hex_string), 2)]))
    if address:
        intermediate.start_addr = {'EIP': address}
    return intermediate


def _write_binary(intermediate: IntelHex, output_file, _address, addr_range):
    if addr_range:
        intermediate.tobinfile(output_file, addr_range[0], addr_range[1])
    else:
        intermediate.tobinfile(output_file)


def _write_ihex(intermediate: IntelHex, output_file, _address, addr_range):
    assert addr_range is None, "Feature not supported for ihex"
    intermediate.tofile(output_file, format='hex')


_formats = ['ihex', 'binary', 'hexstring']


@click.command()
@click.argument('input_file')
@click.argument('output_file')
@click.option('--input_format', default='ihex', help='Input format', type=click.Choice(_formats))
@click.option('--output_format', default='binary', help='Input format', type=click.Choice(_formats))
@click.option('--change-addresses', '--change_addresses', default='0',
              help='Change loading address (only makes sense for ihex output)', type=BASED_INT)
@click.option('--padding-byte', '-p', default=0, help='use a padding byte', type=BASED_INT)
@click.option('--addr-range', '-r', help='select a range, specify start:end addresses')
def main(input_file, output_file, input_format, output_format, change_addresses, padding_byte, addr_range):
    if addr_range:
        addr_range = [int(v, 16) for v in addr_range.split(":")]

    reader_name = f"_read_{input_format}"
    writer_name = f"_write_{output_format}"

    if reader_name not in globals():
        click.echo(f"No reader for {input_format}, abort.", err=True)
        sys.exit(-1)

    if writer_name not in globals():
        click.echo(f"No writer for {output_format}, abort.", err=True)
        sys.exit(-1)

    reader = globals()[reader_name]
    writer = globals()[writer_name]

    intermediate = reader(input_file, change_addresses)

    intermediate.padding = padding_byte

    writer(intermediate, output_file, change_addresses, addr_range)


if __name__ == '__main__':
    main()
