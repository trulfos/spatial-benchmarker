#!/usr/bin/python3
import argparse
from database import Database


def create(db, config_id, definition):
    [name, arguments] = definition.split(':', maxsplit=1)

    db.insert('reporter', config_id=config_id, name=name, arguments=arguments)
    db.commit()


def get(db, benchmark_id):
    rows = db.get_where('reporter', benchmark_id=benchmark_id)
    return sorted(rows, key=lambda r: r['order'])


def print_all(records):
    for r in records:
        print('%(id)d: %(name)s:%(arguments)s' % r)


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Retrieve and create configs from results database'
        )

    parser.add_argument(
            'config_id',
            help='Cofig id or specification'
        )

    parser.add_argument(
            '--add', '-a',
            help='Add reporter'
        )

    parser.add_argument(
            '--delete',
            help='Add reporter'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def main():
    args = parse_arguments()
    config_id = args.config_id
    db = Database(args.database)

    if args.add:
        create(db, config_id, args.add)

    if args.delete:
        db.connection.execute(
                'delete from `reporter` where `reporter_id` = ?',
                [args.delete]
            )

        db.commit()

    print_all(get(db, config_id))


if __name__ == '__main__':
    main()
