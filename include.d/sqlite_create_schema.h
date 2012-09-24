const char *sqlite_create_schema_text = "\
CREATE TABLE book							\
(									\
id		 VARCHAR(32) UNIQUE,					\
    myoid		 BIGINT NOT NULL,				\
    title		 TEXT NOT NULL,					\
    edition		 VARCHAR(8) NOT NULL,				\
    author		 TEXT NOT NULL,					\
    pdate		 VARCHAR(32) NOT NULL,				\
    publisher	 TEXT NOT NULL,						\
    place		 TEXT NOT NULL,					\
    category	 TEXT NOT NULL,						\
    price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
    description	 TEXT NOT NULL,						\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    binding_type	 VARCHAR(32) NOT NULL,				\
    location	 TEXT NOT NULL,						\
    isbn13		 VARCHAR(16) UNIQUE,				\
    lccontrolnumber	 VARCHAR(64),					\
    callnumber	 VARCHAR(64),						\
    deweynumber	 VARCHAR(64),						\
    front_cover	 BYTEA,							\
    back_cover	 BYTEA,							\
    marc_tags    TEXT,							\
    keyword      TEXT,							\
    type		 VARCHAR(16) NOT NULL DEFAULT 'Book'		\
    );									\
									\
CREATE TABLE book_copy_info						\
(									\
item_oid	 BIGINT NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  copyid		 VARCHAR(64) NOT NULL,			\
	  copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	  PRIMARY KEY(item_oid, copyid)					\
  );									\
									\
CREATE TRIGGER book_purge_trigger AFTER DELETE ON book			\
FOR EACH row								\
BEGIN									\
DELETE FROM book_copy_info WHERE item_oid = old.myoid;			\
DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	type = old.type;						\
END;									\
									\
CREATE TABLE cd								\
(									\
id		 VARCHAR(32) NOT NULL PRIMARY KEY,			\
    myoid		 BIGINT NOT NULL,				\
    title		 TEXT NOT NULL,					\
    artist		 TEXT NOT NULL,					\
    recording_label	 TEXT NOT NULL,					\
    rdate		 VARCHAR(32) NOT NULL,				\
    category	 TEXT NOT NULL,						\
    price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
    description	 TEXT NOT NULL,						\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    location	 TEXT NOT NULL,						\
    cdruntime	 VARCHAR(32) NOT NULL,					\
    cdformat	 VARCHAR(128) NOT NULL,					\
    cddiskcount	 INTEGER NOT NULL DEFAULT 1,				\
    cdaudio		 VARCHAR(32) NOT NULL DEFAULT 'Mono',		\
    cdrecording	 VARCHAR(32) NOT NULL DEFAULT 'Live',			\
    front_cover	 BYTEA,							\
    back_cover	 BYTEA,							\
    keyword      TEXT,							\
    type		 VARCHAR(16) NOT NULL DEFAULT 'CD'		\
    );									\
									\
CREATE TABLE cd_songs							\
(									\
item_oid	 BIGINT NOT NULL,					\
	  albumnum	 INTEGER NOT NULL DEFAULT 1,			\
	  songnum		 INTEGER NOT NULL DEFAULT 1,		\
	  songtitle	 VARCHAR(256) NOT NULL,				\
	  runtime		 VARCHAR(32) NOT NULL,			\
	  PRIMARY KEY(item_oid, albumnum, songnum)			\
	  );								\
									\
CREATE TABLE cd_copy_info						\
(									\
item_oid	 BIGINT NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  copyid		 VARCHAR(64) NOT NULL,			\
	  copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	  PRIMARY KEY(item_oid, copyid)					\
	  );								\
									\
CREATE TRIGGER cd_purge_trigger AFTER DELETE ON cd			\
FOR EACH row								\
BEGIN									\
DELETE FROM cd_copy_info WHERE item_oid = old.myoid;			\
DELETE FROM cd_songs WHERE item_oid = old.myoid;			\
DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	  type = old.type;						\
END;									\
									\
CREATE TABLE dvd							\
(									\
id		 VARCHAR(32) NOT NULL PRIMARY KEY,			\
	  myoid		 BIGINT NOT NULL,				\
	  title		 TEXT NOT NULL,					\
	  studio		 TEXT NOT NULL,				\
	  rdate		 VARCHAR(32) NOT NULL,				\
	  category	 TEXT NOT NULL,					\
	  price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
	  description	 TEXT NOT NULL,					\
	  language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
	  monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',\
	  quantity	 INTEGER NOT NULL DEFAULT 1,			\
	  location	 TEXT NOT NULL,					\
	  dvdactor	 TEXT NOT NULL,					\
	  dvdformat	 TEXT NOT NULL,					\
	  dvdruntime	 VARCHAR(32) NOT NULL,				\
	  dvdrating	 VARCHAR(64) NOT NULL,				\
	  dvdregion	 VARCHAR(64) NOT NULL,				\
	  dvddiskcount	 INTEGER NOT NULL DEFAULT 1,			\
	  dvddirector	 TEXT NOT NULL,					\
	  dvdaspectratio	 VARCHAR(64) NOT NULL,			\
	  front_cover	 BYTEA,						\
	  back_cover	 BYTEA,						\
          keyword        TEXT,						\
	  type		 VARCHAR(16) NOT NULL DEFAULT 'DVD'		\
	  );								\
									\
CREATE TABLE dvd_copy_info						\
(									\
item_oid	 BIGINT NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  copyid		 VARCHAR(64) NOT NULL,			\
	  copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	  PRIMARY KEY(item_oid, copyid)					\
	  );								\
									\
CREATE TRIGGER dvd_purge_trigger AFTER DELETE ON dvd			\
FOR EACH row								\
BEGIN									\
DELETE FROM dvd_copy_info WHERE item_oid = old.myoid;			\
DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	  type = old.type;						\
END;									\
									\
CREATE TABLE journal							\
(									\
id		 VARCHAR(32) NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  title		 TEXT NOT NULL,					\
	  pdate		 VARCHAR(32) NOT NULL,				\
	  publisher	 TEXT NOT NULL,					\
	  place		 TEXT NOT NULL,					\
	  category	 TEXT NOT NULL,					\
	  price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
	  description	 TEXT NOT NULL,					\
	  language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
	  monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',\
	  quantity	 INTEGER NOT NULL DEFAULT 1,			\
	  location	 TEXT NOT NULL,					\
	  issuevolume	 INTEGER NOT NULL DEFAULT 0,			\
	  issueno		 INTEGER NOT NULL DEFAULT 0,		\
	  lccontrolnumber	 VARCHAR(64),				\
	  callnumber	 VARCHAR(64),					\
	  deweynumber	 VARCHAR(64),					\
	  front_cover	 BYTEA,						\
	  back_cover	 BYTEA,						\
          marc_tags      TEXT,						\
          keyword        TEXT,						\
	  type		 VARCHAR(16) NOT NULL DEFAULT 'Journal',	\
	  PRIMARY KEY(id, issuevolume, issueno)				\
	  );								\
									\
CREATE TABLE journal_copy_info						\
(									\
item_oid	 BIGINT NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  copyid		 VARCHAR(64) NOT NULL,			\
	  copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	  PRIMARY KEY(item_oid, copyid)					\
	  );								\
									\
CREATE TRIGGER journal_purge_trigger AFTER DELETE ON journal		\
FOR EACH row								\
BEGIN									\
DELETE FROM journal_copy_info WHERE item_oid = old.myoid;		\
DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	  type = old.type;						\
END;									\
									\
CREATE TABLE magazine							\
(									\
id		 VARCHAR(32) NOT NULL,					\
	  myoid		 BIGINT NOT NULL,				\
	  title		 TEXT NOT NULL,					\
	  pdate		 VARCHAR(32) NOT NULL,				\
	  publisher	 TEXT NOT NULL,					\
	  place		 TEXT NOT NULL,					\
	  category	 TEXT NOT NULL,					\
	  price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
	  description	 TEXT NOT NULL,					\
	  language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
	  monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',\
	  quantity	 INTEGER NOT NULL DEFAULT 1,			\
	  location	 TEXT NOT NULL,					\
	  issuevolume	 INTEGER NOT NULL DEFAULT 0,			\
	  issueno		 INTEGER NOT NULL DEFAULT 0,		\
	  lccontrolnumber	 VARCHAR(64),				\
	  callnumber	 VARCHAR(64),					\
	  deweynumber	 VARCHAR(64),					\
	  front_cover	 BYTEA,						\
	  back_cover	 BYTEA,						\
          marc_tags      TEXT,						\
          keyword        TEXT,						\
	  type		 VARCHAR(16) NOT NULL DEFAULT 'Magazine',	\
	  PRIMARY KEY(id, issuevolume, issueno)				\
);									\
									\
CREATE TABLE magazine_copy_info						\
(									\
	item_oid	 BIGINT NOT NULL,				\
	myoid		 BIGINT NOT NULL,				\
	copyid		 VARCHAR(64) NOT NULL,				\
	copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	PRIMARY KEY(item_oid, copyid)					\
);									\
									\
CREATE TRIGGER magazine_purge_trigger AFTER DELETE ON magazine		\
FOR EACH row								\
BEGIN									\
	DELETE FROM magazine_copy_info WHERE item_oid = old.myoid;	\
	DELETE FROM member_history WHERE item_oid = old.myoid AND	\
		type = old.type;					\
END;									\
									\
CREATE TABLE photograph_collection					\
(									\
        id		 TEXT PRIMARY KEY NOT NULL,			\
	myoid		 BIGINT UNIQUE,					\
	title		 TEXT NOT NULL,					\
	about		 TEXT,						\
	notes		 TEXT,						\
	image		 BYTEA,						\
	image_scaled	 BYTEA,						\
	type		 VARCHAR(32) NOT NULL DEFAULT 'Photograph Collection' \
);									\
									\
CREATE TABLE photograph							\
(									\
        id                        TEXT NOT NULL,			\
	myoid			  BIGINT UNIQUE,			\
	collection_oid		  BIGINT NOT NULL,			\
	title			  TEXT NOT NULL,			\
	creators		  TEXT NOT NULL,			\
	pdate			  VARCHAR(32) NOT NULL,			\
	quantity		  INTEGER NOT NULL DEFAULT 1,		\
	medium			  TEXT NOT NULL,			\
	reproduction_number  	  TEXT NOT NULL,			\
	copyright		  TEXT NOT NULL,			\
	callnumber		  VARCHAR(64),				\
	other_number		  TEXT,					\
	location		  TEXT NOT NULL,			\
	notes			  TEXT,					\
	subjects		  TEXT,					\
	format			  TEXT,					\
	image			  BYTEA,				\
	image_scaled		  BYTEA,				\
	PRIMARY KEY(id, collection_oid),				\
	FOREIGN KEY(collection_oid) REFERENCES photograph_collection(myoid) ON \
				   DELETE CASCADE			\
);									\
									\
CREATE TABLE videogame							\
(									\
	id		 VARCHAR(32) NOT NULL PRIMARY KEY,		\
	myoid		 BIGINT NOT NULL,				\
	title		 TEXT NOT NULL,					\
	developer	 TEXT NOT NULL,					\
	genre		 TEXT NOT NULL,					\
	rdate		 VARCHAR(32) NOT NULL,				\
	publisher	 TEXT NOT NULL,					\
	place		 TEXT NOT NULL,					\
	price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
	description	 TEXT NOT NULL,					\
	language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
	monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
	quantity	 INTEGER NOT NULL DEFAULT 1,			\
	location	 TEXT NOT NULL,					\
	vgrating	 VARCHAR(64) NOT NULL,				\
	vgplatform	 VARCHAR(64) NOT NULL,				\
	vgmode		 VARCHAR(16) NOT NULL DEFAULT 'Multiplayer',	\
	front_cover	 BYTEA,						\
	back_cover	 BYTEA,						\
        keyword          TEXT,						\
	type		 VARCHAR(16) NOT NULL DEFAULT 'Video Game'	\
);									\
									\
CREATE TABLE videogame_copy_info					\
(									\
	item_oid	 BIGINT NOT NULL,				\
	myoid		 BIGINT NOT NULL,				\
	copyid		 VARCHAR(64) NOT NULL,				\
	copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	PRIMARY KEY(item_oid, copyid)					\
);									\
									\
CREATE TRIGGER videogame_purge_trigger AFTER DELETE ON videogame	\
FOR EACH row								\
BEGIN									\
	DELETE FROM videogame_copy_info WHERE item_oid = old.myoid;	\
	DELETE FROM member_history WHERE item_oid = old.myoid AND	\
		type = old.type;					\
END;									\
									\
CREATE TABLE item_borrower						\
(									\
	item_oid	 BIGINT NOT NULL,				\
	memberid	 VARCHAR(16) NOT NULL,				\
	reserved_date	 VARCHAR(32) NOT NULL,				\
	duedate		 VARCHAR(32) NOT NULL,				\
	myoid		 INTEGER PRIMARY KEY AUTOINCREMENT,		\
	copyid		 VARCHAR(64) NOT NULL,				\
	copy_number	 INTEGER NOT NULL DEFAULT 1,			\
	reserved_by	 VARCHAR(128) NOT NULL,				\
	type		 VARCHAR(16) NOT NULL,				\
	FOREIGN KEY(memberid) REFERENCES member ON DELETE RESTRICT	\
);									\
									\
CREATE TABLE member							\
(									\
	memberid	 VARCHAR(16) NOT NULL PRIMARY KEY DEFAULT 1,	\
	membersince	 VARCHAR(32) NOT NULL,				\
	dob		 VARCHAR(32) NOT NULL,				\
	sex		 VARCHAR(8) NOT NULL DEFAULT 'Female',		\
	first_name	 VARCHAR(128) NOT NULL,				\
	middle_init	 VARCHAR(1),					\
	last_name	 VARCHAR(128) NOT NULL,				\
	telephone_num	 VARCHAR(32),					\
	street		 VARCHAR(256) NOT NULL,				\
	city		 VARCHAR(256) NOT NULL,				\
	state_abbr	 VARCHAR(16) NOT NULL DEFAULT 'N/A',		\
	zip		 VARCHAR(16) NOT NULL,				\
	email		 VARCHAR(128),					\
	expiration_date  VARCHAR(32) NOT NULL,			        \
	overdue_fees     NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
        comments         TEXT,                                          \
	general_registration_number TEXT,	                        \
	memberclass      TEXT						\
);									\
									\
CREATE TABLE member_history						\
(									\
	memberid	 VARCHAR(16) NOT NULL,				\
	item_oid	 BIGINT NOT NULL,				\
	copyid		 VARCHAR(64) NOT NULL,				\
	reserved_date	 VARCHAR(32) NOT NULL,				\
	duedate		 VARCHAR(32) NOT NULL,				\
	returned_date	 VARCHAR(32) NOT NULL,				\
	myoid		 INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,	\
	reserved_by	 VARCHAR(128) NOT NULL,				\
	type		 VARCHAR(16) NOT NULL				\
);									\
									\
CREATE TRIGGER member_history_trigger AFTER DELETE ON member		\
FOR EACH row								\
BEGIN									\
   DELETE FROM member_history WHERE memberid = old.memberid;		\
END;									\
									\
CREATE VIEW item_borrower_vw AS						\
SELECT	 item_oid,							\
	 memberid,							\
	 myoid,								\
	 copyid,							\
	 copy_number,							\
	 reserved_date,							\
	 duedate,							\
	 type								\
FROM	 item_borrower;						        \
									\
CREATE TABLE locations				                        \
(									\
          location	 TEXT NOT NULL,					\
	  type		 VARCHAR(32),					\
	  PRIMARY KEY(location, type)					\
);      								\
									\
CREATE TABLE monetary_units						\
(									\
	monetary_unit	 TEXT NOT NULL PRIMARY KEY                      \
);                                                                      \
									\
CREATE TABLE languages							\
(									\
        language	 TEXT NOT NULL PRIMARY KEY			\
);      								\
									\
CREATE TABLE cd_formats							\
(									\
	cd_format	 TEXT NOT NULL PRIMARY KEY                      \
);                                                                      \
									\
CREATE TABLE dvd_ratings						\
(									\
	dvd_rating	 TEXT NOT NULL PRIMARY KEY                      \
);                                                                      \
									\
CREATE TABLE dvd_aspect_ratios						\
(									\
        dvd_aspect_ratio	 TEXT NOT NULL PRIMARY KEY		\
);                                                                      \
									\
CREATE TABLE dvd_regions						\
(									\
	dvd_region	 TEXT NOT NULL PRIMARY KEY                      \
);                                                                      \
									\
CREATE TABLE minimum_days						\
(									\
	days		 INTEGER NOT NULL,                              \
	type		 VARCHAR(16) NOT NULL PRIMARY KEY               \
);                                                                      \
									\
CREATE TABLE videogame_ratings						\
(									\
	videogame_rating	 TEXT NOT NULL PRIMARY KEY              \
);                                                                      \
									\
CREATE TABLE videogame_platforms					\
(									\
	videogame_platform	 TEXT NOT NULL PRIMARY KEY              \
);                                                                      \
									\
CREATE TABLE sequence							\
(									\
value            INTEGER PRIMARY KEY AUTOINCREMENT			\
);									\
";
