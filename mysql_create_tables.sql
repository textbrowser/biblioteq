CREATE DATABASE xbook_db; USE xbook_db;
CREATE USER xbook@localhost IDENTIFIED BY 'xbook';
CREATE USER xbook_admin@localhost IDENTIFIED BY 'xbook_admin';

CREATE TABLE book
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	title		 LONGTEXT NOT NULL,
	edition		 VARCHAR(8) NOT NULL,
	author		 LONGTEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 LONGTEXT NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 DOUBLE(10, 2) NOT NULL DEFAULT 0.00,
	description	 LONGTEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	binding_type	 VARCHAR(32) NOT NULL,
	location	 LONGTEXT NOT NULL,
	isbn13		 VARCHAR(16) NOT NULL,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	front_cover	 LONGTEXT, /* Future use. */
	back_cover	 LONGTEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'Book'
) engine = InnoDB;

CREATE TABLE book_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE cd
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	title		 LONGTEXT NOT NULL,
	artist		 LONGTEXT NOT NULL,
	recording_label	 LONGTEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 DOUBLE(10, 2) NOT NULL DEFAULT 0.00,
	description	 LONGTEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 LONGTEXT NOT NULL,
	cdruntime	 VARCHAR(32) NOT NULL,
	cdformat	 VARCHAR(128) NOT NULL,
	cddiskcount	 INTEGER NOT NULL DEFAULT 1,
	cdaudio		 VARCHAR(32) NOT NULL DEFAULT 'Mono',
	cdrecording	 VARCHAR(32) NOT NULL DEFAULT 'Live',
	front_cover	 LONGTEXT, /* Future use. */
	back_cover	 LONGTEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'CD'
) engine = InnoDB;

CREATE TABLE cd_songs
(
	item_oid	 INTEGER NOT NULL,
	albumnum	 INTEGER NOT NULL DEFAULT 1,
	songnum		 INTEGER NOT NULL DEFAULT 1,
	songtitle	 VARCHAR(256) NOT NULL,
	runtime		 VARCHAR(32) NOT NULL,
	PRIMARY KEY(item_oid, albumnum, songnum),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE cd_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE dvd
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	title		 LONGTEXT NOT NULL,
	studio		 LONGTEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 DOUBLE(10, 2) NOT NULL DEFAULT 0.00,
	description	 LONGTEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 LONGTEXT NOT NULL,
	dvdactor	 LONGTEXT NOT NULL,
	dvdformat	 LONGTEXT NOT NULL,
	dvdruntime	 VARCHAR(32) NOT NULL,
	dvdrating	 VARCHAR(64) NOT NULL,
	dvdregion	 VARCHAR(64) NOT NULL,
	dvddiskcount	 INTEGER NOT NULL DEFAULT 1,
	dvddirector	 LONGTEXT NOT NULL,
	dvdaspectratio	 VARCHAR(64) NOT NULL,
	front_cover	 LONGTEXT, /* Future use. */
	back_cover	 LONGTEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'DVD'
) engine = InnoDB;

CREATE TABLE dvd_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES dvd(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE magazine
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	title		 LONGTEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 LONGTEXT NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 DOUBLE(10, 2) NOT NULL DEFAULT 0.00,
	description	 LONGTEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 LONGTEXT NOT NULL,
	mag_volume	 INTEGER NOT NULL DEFAULT 0,
	mag_no		 INTEGER NOT NULL DEFAULT 0,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',
	front_cover	 LONGTEXT, /* Future use. */
	back_cover	 LONGTEXT, /* Future use. */
	PRIMARY KEY(id, mag_volume, mag_no)
) engine = InnoDB;

CREATE TABLE magazine_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE videogame
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	title		 LONGTEXT NOT NULL,
	developer	 LONGTEXT NOT NULL,
	genre		 VARCHAR(64) NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	publisher	 LONGTEXT NOT NULL,
	price		 DOUBLE(10, 2) NOT NULL DEFAULT 0.00,
	description	 LONGTEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 LONGTEXT NOT NULL,
	vgrating	 VARCHAR(64) NOT NULL,
	vgplatform	 VARCHAR(64) NOT NULL,
	vgmode		 VARCHAR(16) NOT NULL DEFAULT 'Multiplayer',
	front_cover	 LONGTEXT, /* Future use. */
	back_cover	 LONGTEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'Video Game'
) engine = InnoDB;

CREATE TABLE videogame_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 INTEGER NOT NULL UNIQUE KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES videogame(myoid) ON DELETE CASCADE
) engine = InnoDB;

CREATE TABLE book_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
) engine = InnoDB;

CREATE TABLE cd_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
) engine = InnoDB;

CREATE TABLE dvd_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
) engine = InnoDB;

CREATE TABLE magazine_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL,
) engine = InnoDB;

CREATE TABLE videogame_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
) engine = InnoDB;

CREATE TABLE member
(
	memberid	 VARCHAR(16) NOT NULL PRIMARY KEY DEFAULT 1,
	membersince	 VARCHAR(32) NOT NULL,
	dob		 VARCHAR(32) NOT NULL,
	sex		 VARCHAR(8) NOT NULL DEFAULT 'Female',
	first_name	 VARCHAR(128) NOT NULL,
	middle_init	 VARCHAR(1),
	last_name	 VARCHAR(128) NOT NULL,
	telephone_num	 VARCHAR(32),
	street		 VARCHAR(256) NOT NULL,
	city		 VARCHAR(256) NOT NULL,
	state_abbr	 VARCHAR(2) NOT NULL DEFAULT 'AK',
	zip		 VARCHAR(16) NOT NULL
) engine = InnoDB;

CREATE TABLE admin
(
	username	 VARCHAR(128) NOT NULL PRIMARY KEY,
	roles		 LONGTEXT NOT NULL
) engine = InnoDB;

GRANT SELECT ON admin TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON book TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON book_copy_info TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON book_borrower TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON cd TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON cd_copy_info TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON cd_songs TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON cd_borrower TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON dvd TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON dvd_copy_info TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON dvd_borrower TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON magazine TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON magazine_copy_info TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON magazine_borrower TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON videogame TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON videogame_copy_info TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT SELECT ON videogame_borrower TO xbook@localhost IDENTIFIED BY 'xbook';
GRANT DELETE, SELECT, UPDATE ON xbook_db.* TO xbook_admin@localhost IDENTIFIED BY "xbook_admin";

INSERT INTO admin VALUES ('xbook_admin', 'all');
