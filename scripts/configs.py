#!/usr/bin/python3
import argparse
from database import Database


def create(db, index, data, definitions):
    config_id = db.insert('config', index=index, data=data).lastrowid
    db.insertmany(
            'option',
            ({'name': d[0], 'value': d[1], 'config_id': config_id}
                for d in definitions.items())
        )

    db.commit()

    return config_id


def group(records):
    groups = {}

    for r in records:
        config_id = r[0]
        if config_id not in groups:
            groups[config_id] = {
                    'id': config_id,
                    'index': r[1],
                    'data': r[2],
                    'definitions': {}
                }

        groups[config_id]['definitions'][r[3]] = r[4]

    return list(groups.values())


def get(db, index, data, options):
    where_clause = ' or '.join(['(`name` = ? and `value` = ?)'] * len(options))

    # Collect all params
    params = [index, data]
    for o in options.items():
        params += o

    params += [len(options)]

    # Perform the search
    rows = db.connection.execute(
            """
            select `config_id`, `index`, `data`, `name`, `value`
            from (
                select `config`.*, count(*) as c
                from `config`
                inner join `option` using (`config_id`)
                where `index` = ? and `data` = ? and (%s)
                group by `config_id`
                having c = ?
            ) as `relevant`
            inner join `option` using (`config_id`)
            """ % where_clause,
            params
        ).fetchall()

    return group(rows)


def get_by_id(db, id):
    results = get_all(db, config_id=id)

    if not results:
        return None

    return results[0]


def get_all(db, **kwargs):
    where_clause = ''

    if kwargs:
        where_clause = 'where %s' % (
                ' and '.join('`%s` = ?' % k for k in kwargs.keys())
            )

    rows = db.connection.execute(
            """
            select `config_id`, `index`, `data`, `name`, `value`
            from `config`
            inner join `option` using (`config_id`)
            """ + where_clause,
            list(kwargs.values())
        ).fetchall()

    return group(rows)


def print_all(records):
    for r in records:
        print(
                "-- Config %(id)d (%(index)s, %(data)s) --" % r +
                ''.join("\n %s:\t%s" % c for c in r['definitions'].items()) +
                '\n'
            )


def parse_arguments():
    parser = argparse.ArgumentParser(
            description='Retrieve and create configs from results database'
        )

    parser.add_argument(
            '--create', '-c', action='store_true',
            help='Create a new config'
        )

    parser.add_argument(
            'config', nargs='*',
            help='Cofig id or specification'
        )

    parser.add_argument(
            '--database', '-d', metavar='filename', default='results',
            help='Path to sqlite database file'
        )

    return parser.parse_args()


def main():
    args = parse_arguments()
    db = Database(args.database)

    if not args.config:
        print_all(get_all(db))
        return

    index = args.config[0]
    definitions = dict(d.split('=') for d in args.config[2:])

    if args.create:
        data = args.config[1]
        print(create(db, index, data, definitions))
    elif index.isdecimal():
        if len(definitions):
            print("Warning: Unused arguments")

        result = get_by_id(db, int(index))

        if result:
            print_all([result])
        else:
            print("No such config")
    elif definitions:
        print_all(get(db, index, definitions))
    else:
        print_all(get_all(db, index=index))


if __name__ == '__main__':
    main()
