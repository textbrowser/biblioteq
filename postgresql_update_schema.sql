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

CREATE TABLE item_request
(
	item_oid	 BIGINT NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	requestdate	 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL PRIMARY KEY,
	type		 VARCHAR(16) NOT NULL
);

GRANT DELETE, SELECT ON item_request TO xbook_admin;
GRANT DELETE, SELECT ON public.item_request_myoid_seq TO xbook_admin;

/* Please execute the "Save Changes" function from within the Administrator
   Browser as this will grant the correct privileges to existing administrator
   accounts. */
/* Please also "Save" each member's information through the Members Browser. */
