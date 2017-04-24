#!/usr/bin/python3

# This script compiles a config. Usage:
# ./compile_for.py <config_id>

import argparse
import subprocess
from database import Database
import os
import sys


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Compile an index for the given config.'
        )

    parser.add_argument(
            'config', metavar='config id', type=int,
            help='Configurations to benchmark (ids)'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    parser.add_argument(
            '--builddir', '-b', metavar='path', default='build',
            help='Path to build dir'
        )

    return parser.parse_args()


def compile(db, config_id, stdout=subprocess.DEVNULL):
    config = db.get_by_id('config', config_id)
    index = config['index']

    options = dict(
            (o['name'], o['value']) for o in
            db.get_where('option', config_id=config_id)
        )

    subprocess.check_call(
            ['cmake'] + ['-D%s=%s' % o for o in options.items()] + ['..'],
            stdout=stdout
        )

    subprocess.check_call(
            ['make', 'bench'],
            stdout=stdout
        )

    subprocess.check_call(
            ['make', index],
            stdout=stdout
        )

    # Copy lib to avoid overwriting before load
    os.rename('lib%s.so' % index, 'lib%s.so' % config_id)


def main():
    args = parse_arguments()
    db = Database(args.database)

    os.chdir(args.builddir)

    print("Compiling for config %s" % args.config)
    compile(db, args.config, sys.stdout)


if __name__ == '__main__':
    main()
