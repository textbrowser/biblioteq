CREATE USER xbook_admin PASSWORD 'xbook_admin' CREATEUSER;

CREATE TABLE book
(
	id		 VARCHAR(32) UNIQUE,
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
	isbn13		 VARCHAR(32) UNIQUE,
	lccontrolnumber	 VARCHAR(64),
	callnumber	 VARCHAR(64),
	deweynumber	 VARCHAR(64),
	front_cover	 BYTEA,
	back_cover	 BYTEA,
	marc_tags	 TEXT,
	keyword		 TEXT, 
	condition 	 TEXT,
	originality 	 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Book'
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
	keyword		 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'CD'
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

CREATE TABLE cd_songs
(
	item_oid	 BIGINT NOT NULL,
	albumnum	 INTEGER NOT NULL DEFAULT 1,
	songnum		 INTEGER NOT NULL DEFAULT 1,
	songtitle	 VARCHAR(256) NOT NULL,
	runtime		 VARCHAR(32) NOT NULL,
	artist		 TEXT NOT NULL DEFAULT 'UNKNOWN',
	composer	 TEXT NOT NULL DEFAULT 'UNKNOWN',
	PRIMARY KEY(item_oid, albumnum, songnum),
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
	keyword		 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'DVD'
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
	marc_tags	 TEXT,
	keyword		 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',
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
	marc_tags	 TEXT,
	keyword		 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Magazine',
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

CREATE TABLE videogame
(
	id		 VARCHAR(32) PRIMARY KEY NOT NULL,
	myoid		 BIGSERIAL UNIQUE,
	title		 TEXT NOT NULL,
	developer	 TEXT NOT NULL,
	genre		 TEXT NOT NULL,
	rdate		 VARCHAR(32) NOT NULL,
	publisher	 TEXT NOT NULL,
	place		 TEXT NOT NULL,
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
	keyword		 TEXT,
	type		 VARCHAR(16) NOT NULL DEFAULT 'Video Game'
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
	email		 VARCHAR(128),
	expiration_date	 VARCHAR(32) NOT NULL,
	overdue_fees	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,
	comments	 TEXT,
	general_registration_number	 TEXT,
	memberclass	 TEXT
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
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
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
	type		 VARCHAR(16) NOT NULL,
	FOREIGN KEY(memberid) REFERENCES member ON DELETE RESTRICT
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

CREATE TABLE book_binding_types
(
	binding_type	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE cd_formats
(
	cd_format	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_aspect_ratios
(
	dvd_aspect_ratio	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_ratings
(
	dvd_rating	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE dvd_regions
(
	dvd_region	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE languages
(
	language	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE locations
(
	location	 TEXT NOT NULL,
	type		 VARCHAR(32),
	PRIMARY KEY(location, type)
);

CREATE TABLE monetary_units
(
	monetary_unit	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE minimum_days
(
	days		 INTEGER NOT NULL,
	type		 VARCHAR(16) NOT NULL PRIMARY KEY
);

CREATE TABLE videogame_platforms
(
	videogame_platform	 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE videogame_ratings
(
	videogame_rating	 TEXT NOT NULL PRIMARY KEY
);

CREATE ROLE biblioteq_administrator INHERIT SUPERUSER;
CREATE ROLE biblioteq_circulation INHERIT;
CREATE ROLE biblioteq_librarian INHERIT;
CREATE ROLE biblioteq_membership CREATEROLE INHERIT;
CREATE ROLE biblioteq_patron NOINHERIT;
CREATE ROLE biblioteq_circulation_librarian INHERIT;
CREATE ROLE biblioteq_circulation_membership INHERIT;
CREATE ROLE biblioteq_librarian_membership INHERIT;
CREATE ROLE biblioteq_circulation_librarian_membership INHERIT;

GRANT biblioteq_circulation TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_patron TO biblioteq_administrator WITH ADMIN OPTION;
GRANT biblioteq_patron TO biblioteq_membership WITH ADMIN OPTION;
GRANT biblioteq_circulation TO biblioteq_circulation_librarian WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_circulation_librarian WITH ADMIN OPTION;
GRANT biblioteq_circulation TO biblioteq_circulation_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_circulation_membership WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_circulation TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_librarian TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;
GRANT biblioteq_membership TO biblioteq_circulation_librarian_membership WITH ADMIN OPTION;

GRANT DELETE, INSERT, SELECT, UPDATE ON admin TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON book TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_binding_types TO biblioteq_administrator;
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
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph TO biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph_collection TO biblioteq_administrator;
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
GRANT SELECT, UPDATE, USAGE ON photograph_collection_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON photograph_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON videogame_copy_info_myoid_seq TO biblioteq_administrator;
GRANT SELECT, UPDATE, USAGE ON videogame_myoid_seq TO biblioteq_administrator;
GRANT SELECT, USAGE ON item_request_myoid_seq TO biblioteq_administrator;

GRANT DELETE, INSERT, SELECT, UPDATE ON item_borrower TO biblioteq_circulation;
GRANT DELETE, INSERT, SELECT, UPDATE ON minimum_days TO biblioteq_circulation;
GRANT DELETE, SELECT ON item_request TO biblioteq_circulation;
GRANT INSERT, SELECT, UPDATE ON member_history TO biblioteq_circulation;
GRANT SELECT ON admin TO biblioteq_circulation;
GRANT SELECT ON book TO biblioteq_circulation;
GRANT SELECT ON book_binding_types TO biblioteq_circulation;
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
GRANT SELECT ON photograph TO biblioteq_circulation;
GRANT SELECT ON photograph_collection TO biblioteq_circulation;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_circulation;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_circulation;
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
GRANT DELETE, INSERT, SELECT, UPDATE ON book_binding_types TO biblioteq_librarian;
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
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON photograph_collection TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_copy_info TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_platforms TO biblioteq_librarian;
GRANT DELETE, INSERT, SELECT, UPDATE ON videogame_ratings TO biblioteq_librarian;
GRANT DELETE, SELECT ON member_history TO biblioteq_librarian;
GRANT SELECT ON admin TO biblioteq_librarian;
GRANT SELECT ON item_borrower_vw TO biblioteq_librarian;
GRANT SELECT ON item_request TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON book_myoid_seq TO biblioteq_librarian;
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
GRANT SELECT, UPDATE, USAGE ON photograph_collection_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON photograph_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON videogame_copy_info_myoid_seq TO biblioteq_librarian;
GRANT SELECT, UPDATE, USAGE ON videogame_myoid_seq TO biblioteq_librarian;

GRANT DELETE, INSERT, SELECT, UPDATE ON member TO biblioteq_membership;
GRANT SELECT ON admin TO biblioteq_membership;
GRANT SELECT ON book TO biblioteq_membership;
GRANT SELECT ON book_binding_types TO biblioteq_membership;
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
GRANT SELECT ON photograph TO biblioteq_membership;
GRANT SELECT ON photograph_collection TO biblioteq_membership;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_membership;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_membership;
GRANT SELECT ON videogame TO biblioteq_membership;
GRANT SELECT ON videogame_copy_info TO biblioteq_membership;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_membership;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_membership;
GRANT SELECT ON videogame_platforms TO biblioteq_membership;
GRANT SELECT ON videogame_ratings TO biblioteq_membership;

GRANT DELETE, INSERT, SELECT ON item_request TO biblioteq_patron;
GRANT SELECT ON book TO biblioteq_patron;
GRANT SELECT ON book_binding_types TO biblioteq_patron;
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
GRANT SELECT ON photograph TO biblioteq_patron;
GRANT SELECT ON photograph_collection TO biblioteq_patron;
GRANT SELECT ON photograph_collection_myoid_seq TO biblioteq_patron;
GRANT SELECT ON photograph_myoid_seq TO biblioteq_patron;
GRANT SELECT ON videogame TO biblioteq_patron;
GRANT SELECT ON videogame_copy_info TO biblioteq_patron;
GRANT SELECT ON videogame_copy_info_myoid_seq TO biblioteq_patron;
GRANT SELECT ON videogame_myoid_seq TO biblioteq_patron;
GRANT SELECT ON videogame_platforms TO biblioteq_patron;
GRANT SELECT ON videogame_ratings TO biblioteq_patron;
GRANT SELECT, UPDATE, USAGE ON item_request_myoid_seq TO biblioteq_patron;
REVOKE ALL ON admin FROM biblioteq_patron;

INSERT INTO admin VALUES ('xbook_admin', 'administrator');
GRANT biblioteq_administrator TO xbook_admin WITH ADMIN OPTION;
