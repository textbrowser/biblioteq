CREATE USER xbook PASSWORD 'xbook';
CREATE USER xbook_admin PASSWORD 'xbook_admin';

CREATE TABLE book
(
	id		 VARCHAR(32) NOT NULL PRIMARY KEY,
	myoid		 SERIAL UNIQUE,
	title		 TEXT NOT NULL,
	edition		 VARCHAR(8) NOT NULL,
	author		 TEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	description	 TEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	binding_type	 VARCHAR(32) NOT NULL,
	location	 TEXT NOT NULL,
	isbn13		 VARCHAR(16) NOT NULL,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	front_cover	 TEXT, /* Future use. */
	back_cover	 TEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'Book'
);

CREATE TABLE book_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	UNIQUE(item_oid, copyid, copy_number),
	FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE
);

CREATE TABLE cd
(
	id		 VARCHAR(32) NOT NULL PRIMARY KEY,
	myoid		 SERIAL UNIQUE,
	title		 TEXT NOT NULL,
	artist		 TEXT NOT NULL,
	recording_label	 TEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	description	 TEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 TEXT NOT NULL,
	cdruntime	 VARCHAR(32) NOT NULL,
	cdformat	 VARCHAR(128) NOT NULL,
	cddiskcount	 INTEGER NOT NULL DEFAULT 1,
	cdaudio		 VARCHAR(32) NOT NULL DEFAULT 'Mono',
	cdrecording	 VARCHAR(32) NOT NULL DEFAULT 'Live',
	front_cover	 TEXT, /* Future use. */
	back_cover	 TEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'CD'
);

CREATE TABLE cd_songs
(
	item_oid	 INTEGER NOT NULL,
	albumnum	 INTEGER NOT NULL DEFAULT 1,
	songnum		 INTEGER NOT NULL DEFAULT 1,
	songtitle	 VARCHAR(256) NOT NULL,
	runtime		 VARCHAR(32) NOT NULL,
	PRIMARY KEY(item_oid, albumnum, songnum),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
);

CREATE TABLE cd_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 SERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
);

CREATE TABLE dvd
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 SERIAL UNIQUE,
	title		 TEXT NOT NULL,
	studio		 TEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	description	 TEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 TEXT NOT NULL,
	dvdactor	 TEXT NOT NULL,
	dvdformat	 TEXT NOT NULL,
	dvdruntime	 VARCHAR(32) NOT NULL,
	dvdrating	 VARCHAR(64) NOT NULL,
	dvdregion	 VARCHAR(64) NOT NULL,
	dvddiskcount	 INTEGER NOT NULL DEFAULT 1,
	dvddirector	 TEXT NOT NULL,
	dvdaspectratio	 VARCHAR(64) NOT NULL,
	front_cover	 TEXT, /* Future use. */
	back_cover	 TEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'DVD'
);

CREATE TABLE dvd_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 SERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES dvd(myoid) ON DELETE CASCADE
);

CREATE TABLE magazine
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL UNIQUE,
	title		 TEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	category	 VARCHAR(64) NOT NULL,
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	description	 TEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 TEXT NOT NULL,
	mag_volume	 INTEGER NOT NULL DEFAULT 0,
	mag_no		 INTEGER NOT NULL DEFAULT 0,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',
	front_cover	 TEXT, /* Future use. */
	back_cover	 TEXT, /* Future use. */
	PRIMARY KEY(id, mag_volume, mag_no)
);

CREATE TABLE magazine_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 SERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE
);

CREATE TABLE videogame
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 SERIAL UNIQUE,
	title		 TEXT NOT NULL,
	developer	 TEXT NOT NULL,
	genre		 VARCHAR(64) NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	description	 TEXT NOT NULL,
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',
	quantity	 INTEGER NOT NULL DEFAULT 1,
	location	 TEXT NOT NULL,
	vgrating	 VARCHAR(64) NOT NULL,
	vgplatform	 VARCHAR(64) NOT NULL,
	vgmode		 VARCHAR(16) NOT NULL DEFAULT 'Multiplayer',
	front_cover	 TEXT, /* Future use. */
	back_cover	 TEXT, /* Future use. */
	type		 VARCHAR(16) NOT NULL DEFAULT 'Video Game'
);

CREATE TABLE videogame_copy_info
(
	item_oid	 INTEGER NOT NULL,
	myoid		 SERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES videogame(myoid) ON DELETE CASCADE
);

CREATE TABLE book_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
);

CREATE TABLE cd_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
);

CREATE TABLE dvd_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
);

CREATE TABLE magazine_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
);

CREATE TABLE videogame_borrower
(
	item_oid	 INTEGER NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL
);

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
);

CREATE TABLE member_history
(
	memberid	 VARCHAR(16) NOT NULL,
	item_oid	 INTEGER NOT NULL,
	copyid		 VARCHAR(64) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	returned_date	 VARCHAR(32) NOT NULL,
	myoid		 SERIAL PRIMARY KEY,
	reserved_by	 VARCHAR(128) NOT NULL,
	type		 VARCHAR(16) NOT NULL
);

CREATE TABLE admin
(
	username	 VARCHAR(128) NOT NULL PRIMARY KEY,
	roles		 TEXT NOT NULL
);

GRANT SELECT ON admin TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON admin TO xbook_admin;
GRANT SELECT ON book TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON book TO xbook_admin;
GRANT SELECT ON public.book_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.book_myoid_seq TO xbook_admin;
GRANT SELECT ON book_copy_info TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_copy_info TO xbook_admin;
GRANT SELECT ON public.book_copy_info_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.book_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT ON cd TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd TO xbook_admin;
GRANT SELECT ON public.cd_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.cd_myoid_seq TO xbook_admin;
GRANT SELECT ON cd_songs TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_songs TO xbook_admin;
GRANT SELECT ON cd_copy_info TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_copy_info TO xbook_admin;
GRANT SELECT ON public.cd_copy_info_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.cd_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT ON dvd TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd TO xbook_admin;
GRANT SELECT ON public.dvd_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.dvd_myoid_seq TO xbook_admin;
GRANT SELECT ON dvd_copy_info TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_copy_info TO xbook_admin;
GRANT SELECT ON public.dvd_copy_info_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.dvd_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT ON magazine TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine TO xbook_admin;
GRANT SELECT ON public.magazine_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.magazine_myoid_seq TO xbook_admin;
GRANT SELECT ON magazine_copy_info TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_copy_info TO xbook_admin;
GRANT SELECT ON public.magazine_copy_info_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.magazine_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT ON videogame TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame TO xbook_admin;
GRANT SELECT ON public.videogame_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.videogame_myoid_seq TO xbook_admin;
GRANT SELECT ON videogame_copy_info TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_copy_info TO xbook_admin;
GRANT SELECT ON public.videogame_copy_info_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.videogame_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT ON book_borrower TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_borrower TO xbook_admin;
GRANT SELECT ON public.book_borrower_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.book_borrower_myoid_seq TO xbook_admin;
GRANT SELECT ON cd_borrower TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_borrower TO xbook_admin;
GRANT SELECT ON public.cd_borrower_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.cd_borrower_myoid_seq TO xbook_admin;
GRANT SELECT ON dvd_borrower TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_borrower TO xbook_admin;
GRANT SELECT ON public.dvd_borrower_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.dvd_borrower_myoid_seq TO xbook_admin;
GRANT SELECT ON magazine_borrower TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_borrower TO xbook_admin;
GRANT SELECT ON public.magazine_borrower_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.magazine_borrower_myoid_seq TO xbook_admin;
GRANT SELECT ON videogame_borrower TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_borrower TO xbook_admin;
GRANT SELECT ON public.videogame_borrower_myoid_seq TO xbook;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.videogame_borrower_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON member TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON member_history TO xbook_admin;

INSERT INTO admin VALUES ('xbook_admin', 'all');
