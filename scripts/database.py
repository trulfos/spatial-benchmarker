#!/usr/bin/python3
import os
import sqlite3


class Database:
    """
    Represents a results database
    """

    def __init__(self, filename=None):
        """
        Opens database with the given filename (or default filename if none is
        given).
        """
        if filename is None:
            filename = 'results'

        # Check if database exists and create it if not
        exists = os.path.exists(filename)
        self.connection = sqlite3.connect(filename)

        if not exists:
            self.create_database()

    def create_database(self):
        """
        Creates the necessary tables
        """
        statements = [
                """
                create table `config` (
                    `config_id` integer primary key autoincrement,
                    `index` text not null,
                    `data` text not null
                )
                """,
                """
                create table `run` (
                    `run_id` integer primary key autoincrement,
                    `config_id` integer not null,
                    `timestamp` datetime default current_timestamp not null,
                    `commit` text not null,

                    foreign key (`config_id`) references `config` (`id`)
                )
                """,
                """
                create table `option` (
                    `config_id` integer not null,
                    `name` text not null,
                    `value` text not null,

                    primary key (`config_id`, `name`),
                    foreign key (`config_id`) references `config` (`id`)
                )
                """,
                """
                create table `reporter` (
                    `reporter_id` integer primary key autoincrement,
                    `config_id` integer not null,
                    `name` text not null,
                    `arguments` text not null,

                    unique (`config_id`, `name`, `arguments`),
                    foreign key (`config_id`) references `config` (`id`)
                )
                """,
                """
                create table `result` (
                    `run_id` integer not null,
                    `reporter_id` integer not null,
                    `name` text not null,
                    `value` real not null,

                    foreign key (`run_id`) references `run` (`id`)
                )
                """
            ]

        for s in statements:
            self.connection.execute(s)

    def create_insert_statement(self, table, keys):
        return 'insert into `%s` (%s) values (%s)' % (
                table,
                ', '.join('`%s`' % k for k in keys),
                ', '.join(':%s' % k for k in keys)
            )

    def insert(self, table, **kwargs):
        """
        Inserts a row in the given table.
        """
        return self.connection.execute(
                self.create_insert_statement(table, kwargs.keys()),
                kwargs
            )

    def insertmany(self, table, data):
        """
        Inserts all rows given in the table
        """
        data = list(data)

        if len(data) < 1:
            return

        return self.connection.executemany(
                self.create_insert_statement(table, data[0].keys()),
                data
            )

    def commit(self):
        self.connection.commit()

    def get_by_id(self, table, id):
        return self.connection.execute(
                'select * from `%s` where `id` = ?' % table,
                [id]
            ).fetchone()

    def get_all(self, table):
        return self.connection.execute(
                'select * from `%s`' % table
            ).fetchall()
