#!/usr/bin/python3
from database import Database
import argparse


def create(db, index, options):
    config_id = db.insert('config', index=index)

    db.insertmany(
            'option',
            [
                {'name': k, 'value': v, 'config_id': config_id}
                for k, v in options.items()
            ]
        )
    db.commit()

    return config_id


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Run benchmarker and insert results in SQLite file'
        )

    parser.add_argument(
            'index', metavar='index',
            help='Index for the new config'
        )

    parser.add_argument(
            'options', metavar='name=value', nargs='+',
            help='Options to insert for the new config'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def main():
    args = parse_arguments()
    db = Database(args.database)
    index = args.index
    options = dict(o.split('=') for o in args.options)

    config_id = create(db, index, options)
    print('New config with id %s' % config_id)


if __name__ == '__main__':
    main()
