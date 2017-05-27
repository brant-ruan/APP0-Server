
drop database if exists APP0;
create database APP0;

use APP0;

drop table if exists User;
create table User(
	usernmae char(16) primary key not null,
	password char(32) not null
);


