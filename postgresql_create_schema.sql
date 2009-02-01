CREATE USER xbook_admin PASSWORD 'xbook_admin' createuser;

CREATE TABLE book
(
	id		 VARCHAR(32) NOT NULL PRIMARY KEY,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	edition		 VARCHAR(8) NOT NULL,
	author		 TEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	place		 TEXT NOT NULL,
	category	 TEXT NOT NULL,
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
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Book',
	offsystem_url	 TEXT
);

CREATE TABLE book_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE
);

CREATE TABLE cd
(
	id		 VARCHAR(32) NOT NULL PRIMARY KEY,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	artist		 TEXT NOT NULL,
	recording_label	 TEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 TEXT NOT NULL,
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
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'CD',
	offsystem_url	 TEXT
);

CREATE TABLE cd_songs
(
	item_oid	 BIGINT NOT NULL,
	albumnum	 INTEGER NOT NULL DEFAULT 1,
	songnum		 INTEGER NOT NULL DEFAULT 1,
	songtitle	 VARCHAR(256) NOT NULL,
	runtime		 VARCHAR(32) NOT NULL,
	PRIMARY KEY(item_oid, albumnum, songnum),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
);

CREATE TABLE cd_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE
);

CREATE TABLE dvd
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	studio		 TEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	category	 TEXT NOT NULL,
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
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'DVD',
	offsystem_url	 TEXT
);

CREATE TABLE dvd_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES dvd(myoid) ON DELETE CASCADE
);

CREATE TABLE journal
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	place		 TEXT NOT NULL,
	category	 TEXT NOT NULL,
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

CREATE TABLE magazine
(
	id		 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	pdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	place		 TEXT NOT NULL,
	category	 TEXT NOT NULL,
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
	type		 VARCHAR(16) NOT NULL DEFAULT 'Magazine',
	offsystem_url    TEXT,
	PRIMARY KEY(id, issuevolume, issueno)
);

CREATE TABLE magazine_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE
);

CREATE TABLE videogame
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	developer	 TEXT NOT NULL,
	genre		 TEXT NOT NULL,
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
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Video Game',
	offsystem_url	 TEXT
);

CREATE TABLE videogame_copy_info
(
	item_oid	 BIGINT NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	copyid		 VARCHAR(64) NOT NULL,
	copy_number	 INTEGER NOT NULL DEFAULT 1,
	PRIMARY KEY(item_oid, copyid),
	FOREIGN KEY(item_oid) REFERENCES videogame(myoid) ON DELETE CASCADE
);

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
	state_abbr	 VARCHAR(16) NOT NULL DEFAULT 'N/A',
	zip		 VARCHAR(16) NOT NULL,
	email		 VARCHAR(128)
);

CREATE TABLE member_history
(
	memberid	 VARCHAR(16) NOT NULL,
	item_oid	 BIGINT NOT NULL,
	copyid		 VARCHAR(64) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	returned_date	 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL PRIMARY KEY,
	reserved_by	 VARCHAR(128) NOT NULL,
	type		 VARCHAR(16) NOT NULL,
	item_id		 VARCHAR(32) NOT NULL,
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
);

CREATE TABLE item_request
(
	item_oid	 BIGINT NOT NULL,
	memberid	 VARCHAR(16) NOT NULL,
	requestdate	 VARCHAR(32) NOT NULL,
	myoid		 BIGSERIAL NOT NULL,
	type		 VARCHAR(16) NOT NULL,
	PRIMARY KEY(item_oid, memberid, type),
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
);

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

CREATE OR REPLACE FUNCTION delete_request() RETURNS trigger AS '
BEGIN
	DELETE FROM item_request WHERE item_oid = new.item_oid AND
	type = new.type;
	RETURN NULL;
END;
' LANGUAGE plpgsql;
CREATE TRIGGER item_request_trigger AFTER INSERT ON item_borrower
FOR EACH row EXECUTE PROCEDURE delete_request();

CREATE TABLE admin
(
	username	 VARCHAR(128) NOT NULL PRIMARY KEY,
	roles		 TEXT NOT NULL
);

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
GRANT DELETE, INSERT, SELECT, UPDATE ON admin TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON book TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.book_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.book_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.cd_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_songs TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON cd_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.cd_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.dvd_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON dvd_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.dvd_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.journal_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON journal_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.journal_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.magazine_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON magazine_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.magazine_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.videogame_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_copy_info TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.videogame_copy_info_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON item_borrower TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.item_borrower_myoid_seq TO xbook_admin;
GRANT DELETE, SELECT ON item_request TO xbook_admin;
GRANT DELETE, SELECT ON public.item_request_myoid_seq TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON member TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON member_history TO xbook_admin;
GRANT DELETE, INSERT, SELECT, UPDATE ON public.member_history_myoid_seq TO xbook_admin;

INSERT INTO admin VALUES ('xbook_admin', 'administrator');
