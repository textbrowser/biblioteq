/* Release 4.04 */

ALTER TABLE book ADD offsystem_url TEXT;
ALTER TABLE cd ADD offsystem_url TEXT;
ALTER TABLE dvd ADD offsystem_url TEXT;
ALTER TABLE magazine ADD offsystem_url TEXT;
ALTER TABLE videogame ADD offsystem_url TEXT;

/* Release 5.00 */
/* Please recreate the database. */

/* Release 5.01 */

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

/* Release 6.00 */

CREATE TABLE member_tmp
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
	state_abbr	 VARCHAR(16) NOT NULL DEFAULT 'N/A',
	zip		 VARCHAR(16) NOT NULL
);

INSERT INTO member_tmp (SELECT * FROM member);
DROP TABLE member;
ALTER TABLE member_tmp RENAME TO member;

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

DROP TABLE cd_borrower;
DROP TABLE dvd_borrower;
DROP TABLE book_borrower;
DROP TABLE magazine_borrower;
DROP TABLE videogame_borrower;

/* Release 6.03 */

DROP TRIGGER book_purge_trigger;
DROP TRIGGER cd_purge_trigger;
DROP TRIGGER dvd_purge_trigger;
DROP TRIGGER magazine_purge_trigger;
DROP TRIGGER videogame_purge_trigger;

CREATE TRIGGER book_purge_trigger AFTER DELETE ON book
FOR EACH row
BEGIN
	DELETE FROM book_copy_info WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;

CREATE TRIGGER cd_purge_trigger AFTER DELETE ON cd
FOR EACH row
BEGIN
	DELETE FROM cd_copy_info WHERE item_oid = old.myoid;
	DELETE FROM cd_songs WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;

CREATE TRIGGER dvd_purge_trigger AFTER DELETE ON dvd
FOR EACH row
BEGIN
	DELETE FROM dvd_copy_info WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;

CREATE TRIGGER magazine_purge_trigger AFTER DELETE ON magazine
FOR EACH row
BEGIN
	DELETE FROM magazine_copy_info WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;

CREATE TRIGGER videogame_purge_trigger AFTER DELETE ON videogame
FOR EACH row
BEGIN
	DELETE FROM videogame_copy_info WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;

CREATE TABLE journal
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 BIGINT NOT NULL,
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
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',
	offsystem_url	 TEXT,
	PRIMARY KEY(id, mag_volume, mag_no)
);

CREATE TABLE journal_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGINT NOT NULL,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid)
);

CREATE TRIGGER journal_purge_trigger AFTER DELETE ON journal
FOR EACH row
BEGIN
	DELETE FROM journal_copy_info WHERE item_oid = old.myoid;
	DELETE FROM member_history WHERE item_oid = old.myoid AND
		type = old.type;
END;
