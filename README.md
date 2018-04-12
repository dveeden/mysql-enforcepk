## Description

The plugin is to prevent creation of tables without a primary key.

See also:

* [Bug #69223 Give a warning on CREATE TABLE without PRIMARY KEY](https://bugs.mysql.com/bug.php?id=69223)

## Example

With SUPER:
```
mysql> CREATE TABLE t1 (id int unsigned);
Query OK, 0 rows affected, 1 warning (0.05 sec)

Warning (Code 1642): Creating tables without primary key is not recommended
mysql> SHOW CREATE TABLE t1\G
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `id` int(10) unsigned DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1
1 row in set (0.00 sec)

mysql> DROP TABLE t1;
Query OK, 0 rows affected (0.02 sec)

mysql> CREATE TABLE t1 (id int unsigned, primary key (id));
Query OK, 0 rows affected (0.03 sec)

mysql> 
```

Without SUPER:
```
mysql> CREATE TABLE t1 (id int unsigned);
ERROR 3164 (HY000): Creating tables without primary key requires SUPER privilege
mysql> CREATE TABLE t1 (id int unsigned primary key);
Query OK, 0 rows affected (0.03 sec)

mysql> 
```

## Loading

Copy enforcepk.so to `@@plugin_dir`.
Use install(1) to do this when upgrading.

```
mysql> install plugin enforcepk soname 'enforcepk.so';
```

## Uninstall

```
mysql> uninstall plugin enforcepk;
```

