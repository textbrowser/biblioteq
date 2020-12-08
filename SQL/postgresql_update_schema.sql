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

/* Release 6.51 */

ALTER TABLE book ADD keyword TEXT;
ALTER TABLE cd ADD keyword TEXT;
ALTER TABLE dvd ADD keyword TEXT;
ALTER TABLE journal ADD keyword TEXT;
ALTER TABLE magazine ADD keyword TEXT;
ALTER TABLE videogame ADD keyword TEXT;
ALTER TABLE book DROP CONSTRAINT book_pkey;
ALTER TABLE book ALTER COLUMN id DROP NOT NULL;
ALTER TABLE book ADD CONSTRAINT book_id_key UNIQUE(id);
ALTER TABLE book ALTER COLUMN isbn13 DROP NOT NULL;
ALTER TABLE book ADD CONSTRAINT book_isbn13_key UNIQUE(id);

/* Release 6.53 */

ALTER TABLE member ADD overdue_fees NUMERIC(10, 2) NOT NULL DEFAULT 0.00;

/* Release 6.55 */

ALTER TABLE member_history DROP item_id;

CREATE ROLE biblioteq_administrator INHERIT;
CREATE ROLE biblioteq_circulation INHERIT;
CREATE ROLE biblioteq_librarian INHERIT;
CREATE ROLE biblioteq_membership INHERIT;
CREATE ROLE biblioteq_patron NOINHERIT;

GRANT biblioteq_circulation TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_patron TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_patron TO biblioteq_membership WITH ADMIN OPTION;

GRANT DELETE, INSERT, SELECT, UPDATE ON admin TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON book TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_formats TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_songs TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_aspect_ratios TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_ratings TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_regions TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON item_borrower TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON languages TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON locations TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON member TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON member_history TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON minimum_days TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON monetary_units TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_copy_info TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_platforms TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_ratings TO biblioteq_administrator;
GRANT DELETE, SELECT ON item_request TO biblioteq_administrator;
GRANT SELECT ON item_borrower_vw TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON book_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON book_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON cd_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON cd_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON dvd_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON dvd_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON item_borrower_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON journal_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON journal_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON magazine_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON magazine_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON member_history_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON videogame_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON videogame_myoid_seq TO biblioteq_administrator;
GRANT SELECT, USAGE ON item_request_myoid_seq TO biblioteq_administrator;

GRANT DELETE, INSERT, SELECT, UPDATE ON item_borrower TO biblioteq_circulation;
GRANT DELETE, INSERT, SELECT, UPDATE ON minimum_days TO biblioteq_circulation;
GRANT DELETE, SELECT ON item_request TO biblioteq_circulation;
GRANT INSERT, SELECT, UPDATE ON member_history TO biblioteq_circulation;
GRANT SELECT ON admin TO biblioteq_circulation;
GRANT SELECT ON book TO biblioteq_circulation;
GRANT SELECT ON book_copy_info TO biblioteq_circulation;
GRANT SELECT ON book_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON book_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON cd TO biblioteq_circulation;
GRANT SELECT ON cd_copy_info TO biblioteq_circulation;
GRANT SELECT ON cd_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON cd_formats TO biblioteq_circulation;
GRANT SELECT ON cd_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON cd_songs TO biblioteq_circulation;
GRANT SELECT ON dvd TO biblioteq_circulation;
GRANT SELECT ON dvd_aspect_ratios TO biblioteq_circulation;
GRANT SELECT ON dvd_copy_info TO biblioteq_circulation;
GRANT SELECT ON dvd_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON dvd_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON dvd_ratings TO biblioteq_circulation;
GRANT SELECT ON dvd_regions TO biblioteq_circulation;
GRANT SELECT ON item_borrower_vw TO biblioteq_circulation;
GRANT SELECT ON journal TO biblioteq_circulation;
GRANT SELECT ON journal_copy_info TO biblioteq_circulation;
GRANT SELECT ON journal_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON journal_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON languages TO biblioteq_circulation;
GRANT SELECT ON locations TO biblioteq_circulation;
GRANT SELECT ON magazine TO biblioteq_circulation;
GRANT SELECT ON magazine_copy_info TO biblioteq_circulation;
GRANT SELECT ON magazine_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON magazine_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON member TO biblioteq_circulation;
GRANT SELECT ON monetary_units TO biblioteq_circulation;
GRANT SELECT ON videogame TO biblioteq_circulation;
GRANT SELECT ON videogame_copy_info TO biblioteq_circulation;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON videogame_platforms TO biblioteq_circulation;
GRANT SELECT ON videogame_ratings TO biblioteq_circulation;
GRANT SELECT, UPDATE, USAGE ON item_borrower_myoid_seq TO biblioteq_circulation;
GRANT SELECT, UPDATE, USAGE ON item_request_myoid_seq TO biblioteq_circulation;
GRANT SELECT, UPDATE, USAGE ON member_history_myoid_seq TO biblioteq_circulation;

GRANT DELETE, INSERT, SELECT, UPDATE ON book TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_formats TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_songs TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_aspect_ratios TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_ratings TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_regions TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON languages TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON locations TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON minimum_days TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON monetary_units TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_platforms TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_ratings TO biblioteq_librarian;
GRANT SELECT ON admin TO biblioteq_librarian;
GRANT SELECT ON item_borrower_vw TO biblioteq_librarian;
GRANT SELECT ON item_request TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON book_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON cd_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON cd_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON dvd_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON dvd_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON item_request_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON journal_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON journal_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON magazine_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON magazine_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON videogame_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON videogame_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON book_myoid_seq TO biblioteq_librarian;

GRANT DELETE, INSERT, SELECT, UPDATE ON member TO biblioteq_membership;
GRANT SELECT ON admin TO biblioteq_membership;
GRANT SELECT ON book TO biblioteq_membership;
GRANT SELECT ON book_copy_info TO biblioteq_membership;
GRANT SELECT ON book_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON book_myoid_seq TO biblioteq_membership;
GRANT SELECT ON cd TO biblioteq_membership;
GRANT SELECT ON cd_copy_info TO biblioteq_membership;
GRANT SELECT ON cd_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON cd_formats TO biblioteq_membership;
GRANT SELECT ON cd_myoid_seq TO biblioteq_membership;
GRANT SELECT ON cd_songs TO biblioteq_membership;
GRANT SELECT ON dvd TO biblioteq_membership;
GRANT SELECT ON dvd_aspect_ratios TO biblioteq_membership;
GRANT SELECT ON dvd_copy_info TO biblioteq_membership;
GRANT SELECT ON dvd_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON dvd_myoid_seq TO biblioteq_membership;
GRANT SELECT ON dvd_ratings TO biblioteq_membership;
GRANT SELECT ON dvd_regions TO biblioteq_membership;
GRANT SELECT ON item_borrower_vw TO biblioteq_membership;
GRANT SELECT ON journal TO biblioteq_membership;
GRANT SELECT ON journal_copy_info TO biblioteq_membership;
GRANT SELECT ON journal_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON journal_myoid_seq TO biblioteq_membership;
GRANT SELECT ON languages TO biblioteq_membership;
GRANT SELECT ON locations TO biblioteq_membership;
GRANT SELECT ON magazine TO biblioteq_membership;
GRANT SELECT ON magazine_copy_info TO biblioteq_membership;
GRANT SELECT ON magazine_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON magazine_myoid_seq TO biblioteq_membership;
GRANT SELECT ON minimum_days TO biblioteq_membership;
GRANT SELECT ON monetary_units TO biblioteq_membership;
GRANT SELECT ON videogame TO biblioteq_membership;
GRANT SELECT ON videogame_copy_info TO biblioteq_membership;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_membership;
GRANT SELECT ON videogame_platforms TO biblioteq_membership;
GRANT SELECT ON videogame_ratings TO biblioteq_membership;

GRANT DELETE, INSERT, SELECT ON item_request TO biblioteq_patron;
GRANT SELECT ON book TO biblioteq_patron;
GRANT SELECT ON book_copy_info TO biblioteq_patron;
GRANT SELECT ON book_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON book_myoid_seq TO biblioteq_patron;
GRANT SELECT ON cd TO biblioteq_patron;
GRANT SELECT ON cd_copy_info TO biblioteq_patron;
GRANT SELECT ON cd_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON cd_formats TO biblioteq_patron;
GRANT SELECT ON cd_myoid_seq TO biblioteq_patron;
GRANT SELECT ON cd_songs TO biblioteq_patron;
GRANT SELECT ON dvd TO biblioteq_patron;
GRANT SELECT ON dvd_aspect_ratios TO biblioteq_patron;
GRANT SELECT ON dvd_copy_info TO biblioteq_patron;
GRANT SELECT ON dvd_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON dvd_myoid_seq TO biblioteq_patron;
GRANT SELECT ON dvd_ratings TO biblioteq_patron;
GRANT SELECT ON dvd_regions TO biblioteq_patron;
GRANT SELECT ON item_borrower_vw TO biblioteq_patron;
GRANT SELECT ON journal TO biblioteq_patron;
GRANT SELECT ON journal_copy_info TO biblioteq_patron;
GRANT SELECT ON journal_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON journal_myoid_seq TO biblioteq_patron;
GRANT SELECT ON languages TO biblioteq_patron;
GRANT SELECT ON locations TO biblioteq_patron;
GRANT SELECT ON magazine TO biblioteq_patron;
GRANT SELECT ON magazine_copy_info TO biblioteq_patron;
GRANT SELECT ON magazine_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON magazine_myoid_seq TO biblioteq_patron;
GRANT SELECT ON member_history TO biblioteq_patron;
GRANT SELECT ON member_history_myoid_seq TO biblioteq_patron;
GRANT SELECT ON monetary_units TO biblioteq_patron;
GRANT SELECT ON videogame TO biblioteq_patron;
GRANT SELECT ON videogame_copy_info TO biblioteq_patron;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_patron;
GRANT SELECT ON videogame_platforms TO biblioteq_patron;
GRANT SELECT ON videogame_ratings TO biblioteq_patron;
GRANT SELECT, UPDATE, USAGE ON item_request_myoid_seq TO biblioteq_patron;
REVOKE ALL ON admin FROM biblioteq_patron;

GRANT biblioteq_administrator TO xbook_admin WITH ADMIN OPTION;

/* Release 6.56 */

ALTER TABLE member ADD comments TEXT, ADD general_registration_number TEXT, ADD memberclass TEXT;

/* Release 6.57 */

ALTER ROLE biblioteq_administrator INHERIT SUPERUSER;
ALTER ROLE biblioteq_membership CREATEUSER INHERIT;
CREATE ROLE biblioteq_circulation_librarian INHERIT;
CREATE ROLE biblioteq_circulation_membership INHERIT;
CREATE ROLE biblioteq_librarian_membership INHERIT;
CREATE ROLE biblioteq_circulation_librarian_membership INHERIT;
GRANT biblioteq_circulation TO biblioteq_circulation_librarian WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_circulation_librarian WITH ADMIN OPTION;
GRANT biblioteq_circulation TO biblioteq_circulation_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_circulation_membership WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_circulation TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;

/* Release 6.59 */

ALTER TABLE book ALTER COLUMN isbn13 TYPE VARCHAR(32);
ALTER TABLE locations ALTER COLUMN type TYPE VARCHAR(32);

CREATE TABLE photograph_collection
(
	id		 TEXT PRIMARY KEY NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	location	 TEXT NOT NULL,
	about		 TEXT,
	notes		 TEXT,
	image		 BYTEA,
	image_scaled	 BYTEA,
	type		 VARCHAR(32) NOT NULL DEFAULT 'Photograph Collection'
);

CREATE TABLE photograph
(
	id			  TEXT NOT NULL,
	myoid			  BIGSERIAL UNIQUE,
	collection_oid		  BIGINT NOT NULL,
	title			  TEXT NOT NULL,
	creators		  TEXT NOT NULL,
	pdate			  VARCHAR(32) NOT NULL,
	quantity		  INTEGER NOT NULL DEFAULT 1,
	medium			  TEXT NOT NULL,
	reproduction_number  	  TEXT NOT NULL,
	copyright		  TEXT NOT NULL,
	callnumber		  VARCHAR(64),
	other_number		  TEXT,
	notes			  TEXT,
	subjects		  TEXT,
	format			  TEXT,
	image			  BYTEA,
	image_scaled		  BYTEA,
	PRIMARY KEY(id, collection_oid),
	FOREIGN KEY(collection_oid) REFERENCES photograph_collection(myoid) ON
				    DELETE CASCADE
);

GRANT DELETE, INSERT, SELECT, UPDATE ON photograph TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph_collection TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON photograph_collection_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON photograph_myoid_seq TO biblioteq_administrator;
GRANT SELECT ON photograph TO biblioteq_circulation;
GRANT SELECT ON photograph_collection TO biblioteq_circulation;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_circulation;
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph_collection TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON photograph_collection_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON photograph_myoid_seq TO biblioteq_librarian;
GRANT SELECT ON photograph TO biblioteq_membership;
GRANT SELECT ON photograph_collection TO biblioteq_membership;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_membership;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_membership;
GRANT SELECT ON photograph TO biblioteq_patron;
GRANT SELECT ON photograph_collection TO biblioteq_patron;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_patron;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_patron;

/* Release 6.63 */

ALTER TABLE cd_songs ADD artist TEXT NOT NULL DEFAULT 'UNKNOWN', ADD composer TEXT NOT NULL DEFAULT 'UNKNOWN';

/* Release 6.64 */

GRANT DELETE, SELECT ON member_history TO biblioteq_librarian;

/* Release 6.66 */

ALTER TABLE book ADD condition TEXT, ADD originality TEXT;
ALTER TABLE book_copy_info ADD condition TEXT, ADD originality TEXT;

CREATE TABLE book_binding_types
(
	binding_type	 TEXT NOT NULL PRIMARY KEY
);

GRANT DELETE, INSERT, SELECT, UPDATE ON book_binding_types TO biblioteq_administrator;
GRANT SELECT ON book_binding_types TO biblioteq_circulation;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_binding_types TO biblioteq_librarian;
GRANT SELECT ON book_binding_types TO biblioteq_membership;
GRANT SELECT ON book_binding_types TO biblioteq_patron;

/* Release 6.67 */

ALTER TABLE locations ALTER type SET NOT NULL;

/* Release 6.70 */

ALTER TABLE member ALTER COLUMN sex TYPE VARCHAR(32);
ALTER TABLE member ALTER zip SET DEFAULT 'N/A';

/* Release 6.71 */

ALTER TABLE journal DROP CONSTRAINT journal_pkey;
ALTER TABLE journal ALTER COLUMN id DROP NOT NULL;
ALTER TABLE journal ADD CONSTRAINT journal_unique_key UNIQUE(id, issuevolume, issueno);
ALTER TABLE magazine DROP CONSTRAINT magazine_pkey;
ALTER TABLE magazine ALTER COLUMN id DROP NOT NULL;
ALTER TABLE magazine ADD CONSTRAINT magazine_unique_key UNIQUE(id, issuevolume, issueno);

/* Release 10.01 */

ALTER TABLE member ALTER COLUMN memberid DROP DEFAULT;
CREATE ROLE biblioteq_guest NOINHERIT;
GRANT SELECT ON book TO biblioteq_guest;
GRANT SELECT ON book_binding_types TO biblioteq_guest;
GRANT SELECT ON book_copy_info TO biblioteq_guest;
GRANT SELECT ON book_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON book_myoid_seq TO biblioteq_guest;
GRANT SELECT ON cd TO biblioteq_guest;
GRANT SELECT ON cd_copy_info TO biblioteq_guest;
GRANT SELECT ON cd_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON cd_formats TO biblioteq_guest;
GRANT SELECT ON cd_myoid_seq TO biblioteq_guest;
GRANT SELECT ON cd_songs TO biblioteq_guest;
GRANT SELECT ON dvd TO biblioteq_guest;
GRANT SELECT ON dvd_aspect_ratios TO biblioteq_guest;
GRANT SELECT ON dvd_copy_info TO biblioteq_guest;
GRANT SELECT ON dvd_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON dvd_myoid_seq TO biblioteq_guest;
GRANT SELECT ON dvd_ratings TO biblioteq_guest;
GRANT SELECT ON dvd_regions TO biblioteq_guest;
GRANT SELECT ON item_borrower_vw TO biblioteq_guest;
GRANT SELECT ON journal TO biblioteq_guest;
GRANT SELECT ON journal_copy_info TO biblioteq_guest;
GRANT SELECT ON journal_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON journal_myoid_seq TO biblioteq_guest;
GRANT SELECT ON languages TO biblioteq_guest;
GRANT SELECT ON locations TO biblioteq_guest;
GRANT SELECT ON magazine TO biblioteq_guest;
GRANT SELECT ON magazine_copy_info TO biblioteq_guest;
GRANT SELECT ON magazine_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON magazine_myoid_seq TO biblioteq_guest;
GRANT SELECT ON monetary_units TO biblioteq_guest;
GRANT SELECT ON photograph TO biblioteq_guest;
GRANT SELECT ON photograph_collection TO biblioteq_guest;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_guest;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_guest;
GRANT SELECT ON videogame TO biblioteq_guest;
GRANT SELECT ON videogame_copy_info TO biblioteq_guest;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_guest;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_guest;
GRANT SELECT ON videogame_platforms TO biblioteq_guest;
GRANT SELECT ON videogame_ratings TO biblioteq_guest;
REVOKE ALL ON admin FROM biblioteq_guest;
REVOKE biblioteq_patron FROM biblioteq_membership;
CREATE USER xbook_guest ENCRYPTED PASSWORD 'xbook_guest' IN ROLE biblioteq_guest;

CREATE TABLE member_history_dnt
(
	memberid	VARCHAR(16) NOT NULL,
	dnt		INTEGER NOT NULL DEFAULT 1,
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
);

GRANT DELETE, SELECT ON member_history_dnt TO biblioteq_administrator;
GRANT SELECT ON member_history_dnt TO biblioteq_circulation;
GRANT DELETE ON member_history_dnt TO biblioteq_membership;
GRANT INSERT, SELECT, UPDATE ON member_history_dnt TO biblioteq_patron;

/* Release 2015.07.04 */

ALTER TABLE member ALTER sex SET DEFAULT 'Private';

/* Release 2016.02.17 */

/* Incomplete, please do not execute. */

CREATE TABLE grey_literature
(
	author		TEXT NOT NULL,
	client		TEXT,
	document_code_a	TEXT NOT NULL,
	document_code_b TEXT NOT NULL,
	document_date	TEXT NOT NULL,
	document_id	TEXT NOT NULL PRIMARY KEY,
	document_status TEXT,
	document_title	TEXT NOT NULL,
	document_type	TEXT NOT NULL,
	front_cover     BYTEA,
	job_number	TEXT NOT NULL,
	location	TEXT,
	myoid		BIGSERIAL UNIQUE,
	notes		TEXT,
	type		VARCHAR(16) NOT NULL DEFAULT 'Grey Literature'
);

GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature TO biblioteq_librarian;
GRANT SELECT ON grey_literature TO biblioteq_circulation;
GRANT SELECT ON grey_literature TO biblioteq_guest;
GRANT SELECT ON grey_literature TO biblioteq_membership;
GRANT SELECT ON grey_literature TO biblioteq_patron;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_membership;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_patron;
GRANT SELECT, UPDATE, USAGE ON grey_literature_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON grey_literature_myoid_seq TO biblioteq_librarian;

/* Release 2016.07.04 */

CREATE TABLE book_files
(
	description	TEXT,
	file		BYTEA NOT NULL,
	file_digest	TEXT NOT NULL,
	file_name	TEXT NOT NULL,
	item_oid	BIGINT NOT NULL,
	myoid		BIGSERIAL NOT NULL,
	FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE,
	PRIMARY KEY(file_digest, item_oid)
);

GRANT DELETE, INSERT, SELECT, UPDATE ON book_files TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON book_files_myoid_seq TO biblioteq_administrator;
GRANT SELECT ON book_files TO biblioteq_circulation;
GRANT SELECT ON book_files_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON book_files TO biblioteq_guest;
GRANT SELECT ON book_files_myoid_seq TO biblioteq_guest;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_files TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON book_files_myoid_seq TO biblioteq_librarian;
GRANT SELECT ON book_files TO biblioteq_membership;
GRANT SELECT ON book_files_myoid_seq TO biblioteq_membership;
GRANT SELECT ON book_files TO biblioteq_patron;
GRANT SELECT ON book_files_myoid_seq TO biblioteq_patron;

CREATE TABLE journal_files
(
	description	TEXT,
	file		BYTEA NOT NULL,
	file_digest	TEXT NOT NULL,
	file_name	TEXT NOT NULL,
	item_oid	BIGINT NOT NULL,
	myoid		BIGSERIAL NOT NULL,
	FOREIGN KEY(item_oid) REFERENCES journal(myoid) ON DELETE CASCADE,
	PRIMARY KEY(file_digest, item_oid)
);

GRANT DELETE, INSERT, SELECT, UPDATE ON journal_files TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON journal_files_myoid_seq TO biblioteq_administrator;
GRANT SELECT ON journal_files TO biblioteq_circulation;
GRANT SELECT ON journal_files_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON journal_files TO biblioteq_guest;
GRANT SELECT ON journal_files_myoid_seq TO biblioteq_guest;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal_files TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON journal_files_myoid_seq TO biblioteq_librarian;
GRANT SELECT ON journal_files TO biblioteq_membership;
GRANT SELECT ON journal_files_myoid_seq TO biblioteq_membership;
GRANT SELECT ON journal_files TO biblioteq_patron;
GRANT SELECT ON journal_files_myoid_seq TO biblioteq_patron;

CREATE TABLE magazine_files
(
	description	TEXT,
	file		BYTEA NOT NULL,
	file_digest	TEXT NOT NULL,
	file_name	TEXT NOT NULL,
	item_oid	BIGINT NOT NULL,
	myoid		BIGSERIAL NOT NULL,
	FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE,
	PRIMARY KEY(file_digest, item_oid)
);

GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_files TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON magazine_files_myoid_seq TO biblioteq_administrator;
GRANT SELECT ON magazine_files TO biblioteq_circulation;
GRANT SELECT ON magazine_files_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON magazine_files TO biblioteq_guest;
GRANT SELECT ON magazine_files_myoid_seq TO biblioteq_guest;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_files TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON magazine_files_myoid_seq TO biblioteq_librarian;
GRANT SELECT ON magazine_files TO biblioteq_membership;
GRANT SELECT ON magazine_files_myoid_seq TO biblioteq_membership;
GRANT SELECT ON magazine_files TO biblioteq_patron;
GRANT SELECT ON magazine_files_myoid_seq TO biblioteq_patron;

REVOKE ALL ON item_borrower_vw FROM biblioteq_guest;
GRANT SELECT (item_oid, type) ON item_borrower_vw TO biblioteq_guest;

/* Release 2016.11.05 */

GRANT SELECT, UPDATE, USAGE ON grey_literature_myoid_seq TO biblioteq_librarian;

/* Release 2016.12.16 */

ALTER TABLE book ADD accession_number TEXT;
ALTER TABLE cd ADD accession_number TEXT;
ALTER TABLE dvd ADD accession_number TEXT;
ALTER TABLE journal ADD accession_number TEXT;
ALTER TABLE magazine ADD accession_number TEXT;
ALTER TABLE photograph ADD accession_number TEXT;
ALTER TABLE photograph_collection ADD accession_number TEXT;
ALTER TABLE videogame ADD accession_number TEXT;

/* Release 2018.04.01 */

CREATE TABLE grey_literature
(
	author		TEXT NOT NULL,
	client		TEXT,
	document_code_a	TEXT NOT NULL,
	document_code_b TEXT NOT NULL,
	document_date	TEXT NOT NULL,
	document_id	TEXT NOT NULL PRIMARY KEY,
	document_status TEXT,
	document_title	TEXT NOT NULL,
	document_type	TEXT NOT NULL,
	job_number	TEXT NOT NULL,
	location	TEXT,
	myoid		BIGSERIAL UNIQUE,
	notes		TEXT,
	type		VARCHAR(16) NOT NULL DEFAULT 'Grey Literature'
);

CREATE TABLE grey_literature_files
(
	description	TEXT,
	file		BYTEA NOT NULL,
	file_digest	TEXT NOT NULL,
	file_name	TEXT NOT NULL,
	item_oid	BIGINT NOT NULL,
	myoid		BIGSERIAL NOT NULL,
	FOREIGN KEY(item_oid) REFERENCES grey_literature(myoid) ON DELETE CASCADE,
	PRIMARY KEY(file_digest, item_oid)
);

CREATE TABLE grey_literature_types
(
	document_type	 TEXT NOT NULL PRIMARY KEY
);

GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature_files TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature_files TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature_types TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON grey_literature_types TO biblioteq_librarian;
GRANT SELECT ON grey_literature TO biblioteq_circulation;
GRANT SELECT ON grey_literature TO biblioteq_guest;
GRANT SELECT ON grey_literature TO biblioteq_membership;
GRANT SELECT ON grey_literature TO biblioteq_patron;
GRANT SELECT ON grey_literature_files TO biblioteq_circulation;
GRANT SELECT ON grey_literature_files TO biblioteq_guest;
GRANT SELECT ON grey_literature_files TO biblioteq_membership;
GRANT SELECT ON grey_literature_files TO biblioteq_patron;
GRANT SELECT ON grey_literature_files_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON grey_literature_files_myoid_seq TO biblioteq_guest;
GRANT SELECT ON grey_literature_files_myoid_seq TO biblioteq_membership;
GRANT SELECT ON grey_literature_files_myoid_seq TO biblioteq_patron;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_guest;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_membership;
GRANT SELECT ON grey_literature_myoid_seq TO biblioteq_patron;
GRANT SELECT ON grey_literature_types TO biblioteq_circulation;
GRANT SELECT ON grey_literature_types TO biblioteq_guest;
GRANT SELECT ON grey_literature_types TO biblioteq_membership;
GRANT SELECT ON grey_literature_types TO biblioteq_patron;
GRANT SELECT, UPDATE, USAGE ON grey_literature_files_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON grey_literature_files_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON grey_literature_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON grey_literature_myoid_seq TO biblioteq_librarian;

/* Release 2018.12.12 */

CREATE EXTENSION IF NOT EXISTS unaccent;

/* Release 2020.07.04 */

CREATE SEQUENCE book_sequence START 1;
DROP TRIGGER IF EXISTS book_trigger ON book;
DROP TRIGGER IF EXISTS cd_trigger ON cd;
DROP TRIGGER IF EXISTS dvd_trigger ON dvd;
DROP TRIGGER IF EXISTS journal_trigger on journal;
DROP TRIGGER IF EXISTS magazine_trigger on magazine;
DROP TRIGGER IF EXISTS videogame_trigger on videogame;
DROP FUNCTION IF EXISTS delete_book_history();
DROP FUNCTION IF EXISTS delete_cd_history();
DROP FUNCTION IF EXISTS delete_dvd_history();
DROP FUNCTION IF EXISTS delete_journal_history();
DROP FUNCTION IF EXISTS delete_magazine_history();
DROP FUNCTION IF EXISTS delete_videogame_history();
DROP VIEW item_borrower_vw;

CREATE OR REPLACE FUNCTION delete_book() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Book'';
	RETURN NULL;
END;
' LANGUAGE plpgsql;
CREATE TRIGGER book_trigger AFTER DELETE ON book
FOR EACH row EXECUTE PROCEDURE delete_book();

CREATE OR REPLACE FUNCTION delete_cd() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''CD'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER cd_trigger AFTER DELETE ON cd
FOR EACH row EXECUTE PROCEDURE delete_cd();

CREATE OR REPLACE FUNCTION delete_dvd() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''DVD'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER dvd_trigger AFTER DELETE ON dvd
FOR EACH row EXECUTE PROCEDURE delete_dvd();

CREATE OR REPLACE FUNCTION delete_journal() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Journal'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER journal_trigger AFTER DELETE ON journal
FOR EACH row EXECUTE PROCEDURE delete_journal();

CREATE OR REPLACE FUNCTION delete_magazine() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Magazine'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER magazine_trigger AFTER DELETE ON magazine
FOR EACH row EXECUTE PROCEDURE delete_magazine();

CREATE OR REPLACE FUNCTION delete_videogame() RETURNS trigger AS '
BEGIN
	DELETE FROM item_borrower WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
	type = ''Video Game'';
	RETURN NULL;
END;
' LANGUAGE 'plpgsql';
CREATE TRIGGER videogame_trigger AFTER DELETE ON videogame
FOR EACH row EXECUTE PROCEDURE delete_videogame();

GRANT SELECT (item_oid, type) ON item_borrower TO biblioteq_guest;
GRANT SELECT, UPDATE, USAGE ON book_sequence TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON book_sequence TO biblioteq_librarian;
GRANT SELECT ON item_borrower TO biblioteq_administrator;
GRANT SELECT ON item_borrower TO biblioteq_circulation;
GRANT SELECT ON item_borrower TO biblioteq_librarian;
GRANT SELECT ON item_borrower TO biblioteq_membership;
GRANT SELECT ON item_borrower TO biblioteq_patron;

/* PostgreSQL 9.5 or newer is required. */

ALTER TABLE item_borrower ENABLE ROW LEVEL SECURITY;
ALTER TABLE item_request ENABLE ROW LEVEL SECURITY;
ALTER TABLE member_history ENABLE ROW LEVEL SECURITY;
ALTER TABLE member_history_dnt ENABLE ROW LEVEL SECURITY;
CREATE POLICY item_borrower_biblioteq_patron_policy ON item_borrower TO biblioteq_patron USING (memberid = session_user);
CREATE POLICY item_borrower_policy ON item_borrower TO biblioteq_administrator, biblioteq_circulation USING (true);
CREATE POLICY item_request_biblioteq_patron_policy ON item_request TO biblioteq_patron USING (memberid = session_user);
CREATE POLICY item_request_policy ON item_request TO biblioteq_administrator, biblioteq_circulation, biblioteq_librarian USING (true);
CREATE POLICY member_history_biblioteq_patron_policy ON member_history TO biblioteq_patron USING (memberid = session_user);
CREATE POLICY member_history_policy ON member_history TO biblioteq_administrator, biblioteq_circulation, biblioteq_librarian, biblioteq_membership USING (true);
CREATE POLICY member_history_dnt_biblioteq_patron_policy ON member_history_dnt TO biblioteq_patron USING (memberid = session_user);
CREATE POLICY member_history_dnt_policy ON member_history_dnt TO biblioteq_administrator, biblioteq_circulation, biblioteq_membership USING (true);

/*
** Release 2021.01.01
*/

ALTER TABLE book_copy_info ADD status TEXT;
ALTER TABLE cd_copy_info ADD status TEXT;
ALTER TABLE dvd_copy_info ADD status TEXT;
ALTER TABLE journal_copy_info ADD status TEXT;
ALTER TABLE magazine_copy_info ADD status TEXT;
ALTER TABLE videogame_copy_info ADD status TEXT;
