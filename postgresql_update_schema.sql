/*
** Please do not load this file in its entirety.
*/

/* Release 3.22 */

ALTER TABLE book ADD type VARCHAR(16);
UPDATE book SET type = 'Book';
ALTER TABLE book ALTER type SET DEFAULT 'Book';
ALTER TABLE book ALTER type SET NOT NULL;

ALTER TABLE cd ADD type VARCHAR(16);
UPDATE cd SET type = 'CD';
ALTER TABLE cd ALTER type SET DEFAULT 'CD';
ALTER TABLE cd ALTER type SET NOT NULL;

ALTER TABLE dvd ADD type VARCHAR(16);
UPDATE dvd SET type = 'DVD';
ALTER TABLE dvd ALTER type SET DEFAULT 'DVD';
ALTER TABLE dvd ALTER type SET NOT NULL;

ALTER TABLE videogame ADD type VARCHAR(16);
UPDATE videogame SET type = 'Video Game';
ALTER TABLE videogame ALTER type SET DEFAULT 'Video Game';
ALTER TABLE videogame ALTER type SET NOT NULL;

/* Releases 3.23, 3.23.1 */

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
	type		 VARCHAR(16) NOT NULL,
	item_id		 VARCHAR(32) NOT NULL,
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
);

GRANT DELETE, INSERT, SELECT, UPDATE ON member_history TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.member_history_myoid_seq TO xbook_admin;

/* Release 4.00 */

CREATE VIEW cd_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 cd_borrower;

CREATE VIEW dvd_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 dvd_borrower;

CREATE VIEW book_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 book_borrower;

CREATE VIEW magazine_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 magazine_borrower;

CREATE VIEW videogame_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 videogame_borrower;

GRANT SELECT ON cd_borrower_vw TO xbook_admin;
GRANT SELECT ON dvd_borrower_vw TO xbook_admin;
GRANT SELECT ON book_borrower_vw TO xbook_admin;
GRANT SELECT ON magazine_borrower_vw TO xbook_admin;
GRANT SELECT ON videogame_borrower_vw TO xbook_admin;
DROP USER xbook;
ALTER USER xbook_admin CREATEUSER;

/* Release 4.01 */

ALTER TABLE book DROP front_cover;
ALTER TABLE book DROP back_cover;
ALTER TABLE book ADD front_cover BYTEA;
ALTER TABLE book ADD back_cover BYTEA;
ALTER TABLE book ADD front_cover_fmt VARCHAR(8);
ALTER TABLE book ADD back_cover_fmt VARCHAR(8);
ALTER TABLE cd DROP front_cover;
ALTER TABLE cd DROP back_cover;
ALTER TABLE cd ADD front_cover BYTEA;
ALTER TABLE cd ADD back_cover BYTEA;
ALTER TABLE cd ADD front_cover_fmt VARCHAR(8);
ALTER TABLE cd ADD back_cover_fmt VARCHAR(8);
ALTER TABLE dvd DROP front_cover;
ALTER TABLE dvd DROP back_cover;
ALTER TABLE dvd ADD front_cover BYTEA;
ALTER TABLE dvd ADD back_cover BYTEA;
ALTER TABLE dvd ADD front_cover_fmt VARCHAR(8);
ALTER TABLE dvd ADD back_cover_fmt VARCHAR(8);
ALTER TABLE magazine DROP front_cover;
ALTER TABLE magazine DROP back_cover;
ALTER TABLE magazine ADD front_cover BYTEA;
ALTER TABLE magazine ADD back_cover BYTEA;
ALTER TABLE magazine ADD front_cover_fmt VARCHAR(8);
ALTER TABLE magazine ADD back_cover_fmt VARCHAR(8);
ALTER TABLE videogame DROP front_cover;
ALTER TABLE videogame DROP back_cover;
ALTER TABLE videogame ADD front_cover BYTEA;
ALTER TABLE videogame ADD back_cover BYTEA;
ALTER TABLE videogame ADD front_cover_fmt VARCHAR(8);
ALTER TABLE videogame ADD back_cover_fmt VARCHAR(8);

/* Release 4.04 */

ALTER TABLE book ADD offsystem_url TEXT;
ALTER TABLE cd ADD offsystem_url TEXT;
ALTER TABLE dvd ADD offsystem_url TEXT;
ALTER TABLE magazine ADD offsystem_url TEXT;
ALTER TABLE videogame ADD offsystem_url TEXT;

/* Release 5.00 */
/* Please recreate the database. */

/* Release 5.01 */

UPDATE admin SET roles = 'administrator' WHERE roles = 'all';

DROP VIEW cd_borrower_vw;
DROP VIEW dvd_borrower_vw;
DROP VIEW book_borrower_vw;
DROP VIEW magazine_borrower_vw;
DROP VIEW videogame_borrower_vw;

CREATE VIEW cd_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 cd_borrower;

CREATE VIEW dvd_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 dvd_borrower;

CREATE VIEW book_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 book_borrower;

CREATE VIEW magazine_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 magazine_borrower;

CREATE VIEW videogame_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 videogame_borrower;

GRANT SELECT ON cd_borrower_vw TO xbook_admin;
GRANT SELECT ON dvd_borrower_vw TO xbook_admin;
GRANT SELECT ON book_borrower_vw TO xbook_admin;
GRANT SELECT ON magazine_borrower_vw TO xbook_admin;
GRANT SELECT ON videogame_borrower_vw TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.member_history_myoid_seq TO xbook_admin;

/* Release 6.00 */

ALTER TABLE member ADD COLUMN new_col VARCHAR(16);
ALTER TABLE member ALTER COLUMN new_col SET DEFAULT 'N/A';
UPDATE member SET new_col = CAST(state_abbr AS VARCHAR(16));
ALTER TABLE member DROP COLUMN state_abbr;
ALTER TABLE member RENAME new_col TO state_abbr;
ALTER TABLE member ALTER COLUMN state_abbr SET NOT NULL;

DROP VIEW cd_borrower_vw;
DROP VIEW dvd_borrower_vw;
DROP VIEW book_borrower_vw;
DROP VIEW magazine_borrower_vw;
DROP VIEW videogame_borrower_vw;

CREATE TABLE item_borrower
(
	item_oid	 BIGINT NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL PRIMARY KEY,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	reserved_by	 VARCHAR(128) NOT NULL,
	type		 VARCHAR(16) NOT NULL
);

CREATE VIEW item_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate,
	 type
FROM	 item_borrower;

INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'CD' FROM cd_borrower;
INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'DVD' FROM dvd_borrower;
INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'Book' FROM book_borrower;
INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'Journal' FROM magazine_borrower WHERE item_oid IN (SELECT myoid FROM magazine WHERE type = 'Journal');
INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'Magazine' FROM magazine_borrower WHERE item_oid IN (SELECT myoid FROM magazine WHERE type = 'Magazine');
INSERT INTO item_borrower SELECT item_oid, memberid, reserved_date, duedate, myoid, copyid, copy_number, reserved_date, 'Video Game' FROM videogame_borrower;

GRANT SELECT ON item_borrower_vw TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON item_borrower TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.item_borrower_myoid_seq TO xbook_admin;

DROP TABLE cd_borrower CASCADE;
DROP TABLE dvd_borrower CASCADE;
DROP TABLE book_borrower CASCADE;
DROP TABLE magazine_borrower CASCADE;
DROP TABLE videogame_borrower CASCADE;

/* Please execute the "Save Changes" function from within the Administrator
   Browser as this will grant the correct privileges to existing administrator
   accounts. */
/* Please also "Save" each member's information through the Members Browser. */

/* Release 6.02 */

DROP VIEW item_borrower_vw;

CREATE VIEW item_borrower_vw AS
SELECT	 item_oid,
	 memberid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate,
	 type
FROM	 item_borrower;

GRANT SELECT ON item_borrower_vw TO xbook_admin;

CREATE TABLE item_request
(
	item_oid	 BIGINT NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	requestdate	 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL NOT NULL,
	type		 VARCHAR(16) NOT NULL,
	PRIMARY KEY(item_oid, type),
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
);

GRANT DELETE, SELECT ON item_request TO xbook_admin;
GRANT DELETE, SELECT ON public.item_request_myoid_seq TO xbook_admin;

CREATE OR REPLACE FUNCTION delete_request() RETURNS trigger AS '
BEGIN
	DELETE FROM item_request WHERE item_oid = new.item_oid AND
	type = new.type;
	RETURN NULL;
END;
' LANGUAGE plpgsql;
CREATE TRIGGER item_request_trigger AFTER INSERT ON item_borrower
FOR EACH row EXECUTE PROCEDURE delete_request();

/* Please execute the "Save Changes" function from within the Administrator
   Browser as this will grant the correct privileges to existing administrator
   accounts. */
/* Please also "Save" each member's information through the Members Browser. */

/* Release 6.03 */

ALTER TABLE magazine RENAME mag_volume TO issuevolume;
ALTER TABLE magazine RENAME mag_no TO issueno;
ALTER TABLE magazine ALTER type SET DEFAULT 'Magazine';

CREATE TABLE journal
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
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
	issuevolume	 INTEGER NOT NULL DEFAULT 0,
	issueno		 INTEGER NOT NULL DEFAULT 0,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',
	offsystem_url    TEXT,
	PRIMARY KEY(id, issuevolume, issueno)
);

CREATE TABLE journal_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES journal(myoid) ON DELETE CASCADE
);

GRANT DELETE, INSERT, SELECT, UPDATE ON journal TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.journal_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.journal_copy_info_myoid_seq TO xbook_admin;

ALTER TABLE book DROP COLUMN front_cover_fmt;
ALTER TABLE book DROP COLUMN back_cover_fmt;
ALTER TABLE cd DROP COLUMN front_cover_fmt;
ALTER TABLE cd DROP COLUMN back_cover_fmt;
ALTER TABLE dvd DROP COLUMN front_cover_fmt;
ALTER TABLE dvd DROP COLUMN back_cover_fmt;
ALTER TABLE magazine DROP COLUMN front_cover_fmt;
ALTER TABLE magazine DROP COLUMN back_cover_fmt;
ALTER TABLE videogame DROP COLUMN front_cover_fmt;
ALTER TABLE videogame DROP COLUMN back_cover_fmt;

DROP TRIGGER book_trigger ON book;
DROP TRIGGER cd_trigger ON cd;
DROP TRIGGER dvd_trigger ON dvd;
DROP TRIGGER magazine_trigger ON magazine;
DROP TRIGGER videogame_trigger ON videogame;

CREATE OR REPLACE FUNCTION delete_book_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Book'';
	RETURN NULL;
END;
' LANGUAGE plpgsql;
CREATE TRIGGER book_trigger AFTER DELETE ON book
FOR EACH row EXECUTE PROCEDURE delete_book_history();

CREATE OR REPLACE FUNCTION delete_cd_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''CD'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER cd_trigger AFTER DELETE ON cd
FOR EACH row EXECUTE PROCEDURE delete_cd_history();

CREATE OR REPLACE FUNCTION delete_dvd_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''DVD'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER dvd_trigger AFTER DELETE ON dvd
FOR EACH row EXECUTE PROCEDURE delete_dvd_history();

CREATE OR REPLACE FUNCTION delete_journal_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Journal'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER journal_trigger AFTER DELETE ON journal
FOR EACH row EXECUTE PROCEDURE delete_journal_history();

CREATE OR REPLACE FUNCTION delete_magazine_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Magazine'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER magazine_trigger AFTER DELETE ON magazine
FOR EACH row EXECUTE PROCEDURE delete_magazine_history();

CREATE OR REPLACE FUNCTION delete_videogame_history() RETURNS trigger AS '
BEGIN
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Video Game'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER videogame_trigger AFTER DELETE ON videogame
FOR EACH row EXECUTE PROCEDURE delete_videogame_history();

INSERT INTO journal (SELECT * FROM magazine WHERE type = 'Journal');
INSERT INTO journal_copy_info (SELECT * FROM magazine_copy_info WHERE item_oid IN (SELECT myoid FROM magazine WHERE type = 'Journal'));
DELETE FROM magazine WHERE type = 'Journal';
DELETE FROM magazine_copy_info WHERE item_oid IN (SELECT myoid FROM magazine WHERE type = 'Journal');

/* Please execute the "Save Changes" function from within the Administrator
   Browser as this will grant the correct privileges to existing administrator
   accounts. */
/* Please also "Save" each member's information through the Members Browser. */

/* Release 6.06 */

ALTER TABLE book ADD COLUMN new_category TEXT;
UPDATE book SET new_category = CAST(category AS TEXT);
ALTER TABLE book DROP COLUMN category;
ALTER TABLE book RENAME new_category TO category;
ALTER TABLE book ALTER COLUMN category SET NOT NULL;

ALTER TABLE cd ADD COLUMN new_category TEXT;
UPDATE cd SET new_category = CAST(category AS TEXT);
ALTER TABLE cd DROP COLUMN category;
ALTER TABLE cd RENAME new_category TO category;
ALTER TABLE cd ALTER COLUMN category SET NOT NULL;

ALTER TABLE dvd ADD COLUMN new_category TEXT;
UPDATE dvd SET new_category = CAST(category AS TEXT);
ALTER TABLE dvd DROP COLUMN category;
ALTER TABLE dvd RENAME new_category TO category;
ALTER TABLE dvd ALTER COLUMN category SET NOT NULL;

ALTER TABLE journal ADD COLUMN new_category TEXT;
UPDATE journal SET new_category = CAST(category AS TEXT);
ALTER TABLE journal DROP COLUMN category;
ALTER TABLE journal RENAME new_category TO category;
ALTER TABLE journal ALTER COLUMN category SET NOT NULL;

ALTER TABLE magazine ADD COLUMN new_category TEXT;
UPDATE magazine SET new_category = CAST(category AS TEXT);
ALTER TABLE magazine DROP COLUMN category;
ALTER TABLE magazine RENAME new_category TO category;
ALTER TABLE magazine ALTER COLUMN category SET NOT NULL;

ALTER TABLE videogame ADD COLUMN new_genre TEXT;
UPDATE videogame SET new_genre = CAST(genre AS TEXT);
ALTER TABLE videogame DROP COLUMN genre;
ALTER TABLE videogame RENAME new_genre TO genre;
ALTER TABLE videogame ALTER COLUMN genre SET NOT NULL;

/* Release 6.12 */

ALTER TABLE member ADD COLUMN email VARCHAR(128);

/* Please "Save" each member's information through the Members Browser. */

/* Release 6.13 */

/* Please "Save" each member's information through the Members Browser. */

/* Release 6.17 */

ALTER TABLE item_request DROP CONSTRAINT item_request_pkey;
ALTER TABLE item_request ADD PRIMARY KEY (item_oid, memberid, type);

/* Release 6.19 */

ALTER TABLE book ADD COLUMN place TEXT;
ALTER TABLE book ALTER place SET DEFAULT 'N/A';
UPDATE book SET place = 'N/A';
ALTER TABLE book ALTER place SET NOT NULL;

ALTER TABLE journal ADD COLUMN place TEXT;
ALTER TABLE journal ALTER place SET DEFAULT 'N/A';
UPDATE journal SET place = 'N/A';
ALTER TABLE journal ALTER place SET NOT NULL;

ALTER TABLE magazine ADD COLUMN place TEXT;
ALTER TABLE magazine ALTER place SET DEFAULT 'N/A';
UPDATE magazine SET place = 'N/A';
ALTER TABLE magazine ALTER place SET NOT NULL;

ALTER TABLE videogame ADD COLUMN place TEXT;
ALTER TABLE videogame ALTER place SET DEFAULT 'N/A';
UPDATE videogame SET place = 'N/A';
ALTER TABLE videogame ALTER place SET NOT NULL;

ALTER TABLE item_borrower ADD FOREIGN KEY(memberid) REFERENCES member ON DELETE RESTRICT;

/* Release 6.30 */

ALTER TABLE book DROP offsystem_url;
ALTER TABLE cd DROP offsystem_url;
ALTER TABLE dvd DROP offsystem_url;
ALTER TABLE journal DROP offsystem_url;
ALTER TABLE magazine DROP offsystem_url;
ALTER TABLE videogame DROP offsystem_url;
VACUUM FULL;

/* Release 6.37 */

UPDATE book SET edition = TRIM(BOTH 'st' FROM edition);
UPDATE book SET edition = TRIM(BOTH 'nd' FROM edition);
UPDATE book SET edition = TRIM(BOTH 'rd' FROM edition);
UPDATE book SET edition = TRIM(BOTH 'th' FROM edition);

/* Release 6.39 */

GRANT SELECT, UPDATE, USAGE ON public.book_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.book_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.cd_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.cd_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.dvd_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.dvd_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.journal_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.journal_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.magazine_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.magazine_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.videogame_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.videogame_copy_info_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.item_borrower_myoid_seq TO xbook_admin;
GRANT SELECT, USAGE ON public.item_request_myoid_seq TO xbook_admin;
GRANT SELECT, UPDATE, USAGE ON public.member_history_myoid_seq TO xbook_admin;

/* Release 6.44 */

CREATE TABLE locations
(
	location	 TEXT NOT NULL,
	type		 VARCHAR(16),
	PRIMARY KEY(location, type)
);

CREATE TABLE monetary_units
(
	monetary_unit	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE languages
(
	language	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE cd_formats
(
	cd_format	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_ratings
(
	dvd_rating	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_aspect_ratios
(
	dvd_aspect_ratio	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_regions
(
	dvd_region	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE minimum_days
(
	days		 INTEGER NOT NULL,
	type		 VARCHAR(16) NOT NULL PRIMARY KEY
);

CREATE TABLE videogame_ratings
(
	videogame_rating	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE videogame_platforms
(
	videogame_platform	 TEXT NOT NULL PRIMARY KEY
);

ALTER TABLE book ADD marc_tags TEXT;
ALTER TABLE journal ADD marc_tags TEXT;
ALTER TABLE magazine ADD marc_tags TEXT;
GRANT DELETE, INSERT, SELECT, UPDATE ON locations TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON monetary_units TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON languages TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_formats TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_ratings TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_aspect_ratios TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_regions TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON minimum_days TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_ratings TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_platforms TO xbook_admin;

/* Please execute the "Save Changes" function from within the Administrator
   Browser as this will grant the correct privileges to existing administrator
   accounts. */
/* Please also "Save" each member's information through the Members Browser. */

/* Release 6.46 */

ALTER TABLE member ADD expiration_date VARCHAR(32) NOT NULL DEFAULT '01/01/3000';
GRANT DELETE, INSERT, SELECT, UPDATE ON member TO xbook_admin;

/* Please "Save" each member's information through the Members Browser. */
