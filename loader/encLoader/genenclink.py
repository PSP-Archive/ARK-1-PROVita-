#!/usr/bin/python

from string import Template
import sys

templ = '''\
OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(encLoader)

SECTIONS
{
  . = $linkaddr;
  .text.startup : {
    *(.text.startup)
  }
  .text : {
    *(.text)
  }
  .rodata : {
    *(.rodata)
  }
  .data : {
    *(.data)
  }
  .bss : {
    __bss_start = .;
    *(.bss)
    __bss_end = .;
  }
}'''

def main():
	if len(sys.argv) < 2:
		print("Usage: %s <link address>" % (sys.argv[0]))
		sys.exit(1)

	t = Template(templ)
	s = t.substitute(linkaddr = sys.argv[1])
	print(s)

if __name__ == "__main__":
	main()
