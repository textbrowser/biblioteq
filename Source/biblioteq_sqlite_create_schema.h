const char *sqlite_create_schema_text = "\
CREATE TABLE book							\
(									\
    author       TEXT NOT NULL,						\
    back_cover	 BYTEA,							\
    binding_type VARCHAR(32) NOT NULL,					\
    callnumber	 VARCHAR(64),						\
    category	 TEXT NOT NULL,						\
    condition    TEXT,							\
    description	 TEXT NOT NULL,						\
    deweynumber	 VARCHAR(64),						\
    edition	 VARCHAR(8) NOT NULL,					\
    front_cover	 BYTEA,							\
    id		 VARCHAR(32) UNIQUE,					\
    isbn13	 VARCHAR(32) UNIQUE,					\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    lccontrolnumber	 VARCHAR(64),					\
    location	 TEXT NOT NULL,						\
    marc_tags    TEXT,							\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    originality  TEXT,							\
    pdate	 VARCHAR(32) NOT NULL,					\
    place	 TEXT NOT NULL,						\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    publisher	 TEXT NOT NULL,						\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'Book'			\
);									\
									\
CREATE TABLE book_copy_info						\
(									\
    copy_number	 INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    condition    TEXT,							\
    item_oid	 BIGINT NOT NULL,					\
    originality  TEXT,							\
    PRIMARY KEY(item_oid, copyid),					\
    FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TABLE book_files							\
(									\
    description	TEXT,							\
    file	BYTEA NOT NULL,						\
    file_digest	TEXT NOT NULL,						\
    file_name   TEXT NOT NULL,						\
    item_oid	BIGINT NOT NULL,					\
    myoid	BIGINT NOT NULL,					\
    FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE,	\
    PRIMARY KEY(file_digest, item_oid)					\
);									\
									\
CREATE TRIGGER book_purge_trigger AFTER DELETE ON book			\
FOR EACH row								\
BEGIN									\
    DELETE FROM book_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
                type = old.type;					\
END;									\
									\
CREATE TABLE cd								\
(									\
    artist	 TEXT NOT NULL,						\
    back_cover	 BYTEA,							\
    category	 TEXT NOT NULL,						\
    cdaudio	 VARCHAR(32) NOT NULL DEFAULT 'Mono',			\
    cddiskcount	 INTEGER NOT NULL DEFAULT 1,				\
    cdformat	 VARCHAR(128) NOT NULL,					\
    cdrecording	 VARCHAR(32) NOT NULL DEFAULT 'Live',			\
    cdruntime	 VARCHAR(32) NOT NULL,					\
    description	 TEXT NOT NULL,						\
    front_cover	 BYTEA,							\
    id		 VARCHAR(32) NOT NULL PRIMARY KEY,			\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    location	 TEXT NOT NULL,						\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    rdate	 VARCHAR(32) NOT NULL,					\
    recording_label	 TEXT NOT NULL,					\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'CD'			\
);									\
									\
CREATE TABLE cd_songs							\
(									\
    albumnum	 INTEGER NOT NULL DEFAULT 1,				\
    artist       TEXT NOT NULL DEFAULT 'UNKNOWN',			\
    composer     TEXT NOT NULL DEFAULT 'UNKNOWN',			\
    item_oid	 BIGINT NOT NULL,					\
    runtime	 VARCHAR(32) NOT NULL,					\
    songnum	 INTEGER NOT NULL DEFAULT 1,				\
    songtitle	 VARCHAR(256) NOT NULL,					\
    PRIMARY KEY(item_oid, albumnum, songnum),				\
    FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TABLE cd_copy_info						\
(									\
    copy_number	 INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    item_oid	 BIGINT NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    PRIMARY KEY(item_oid, copyid),					\
    FOREIGN KEY(item_oid) REFERENCES cd(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TRIGGER cd_purge_trigger AFTER DELETE ON cd			\
FOR EACH row								\
BEGIN									\
    DELETE FROM cd_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM cd_songs WHERE item_oid = old.myoid;			\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
                type = old.type;					\
END;									\
									\
CREATE TABLE dvd							\
(									\
    back_cover	 BYTEA,							\
    category	 TEXT NOT NULL,						\
    description	 TEXT NOT NULL,						\
    dvdactor	 TEXT NOT NULL,						\
    dvdaspectratio	 VARCHAR(64) NOT NULL,				\
    dvddirector	 TEXT NOT NULL,						\
    dvddiskcount INTEGER NOT NULL DEFAULT 1,				\
    dvdformat	 TEXT NOT NULL,						\
    dvdrating	 VARCHAR(64) NOT NULL,					\
    dvdregion	 VARCHAR(64) NOT NULL,					\
    dvdruntime	 VARCHAR(32) NOT NULL,					\
    front_cover	 BYTEA,							\
    id		 VARCHAR(32) NOT NULL PRIMARY KEY,			\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    location	 TEXT NOT NULL,						\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    rdate	 VARCHAR(32) NOT NULL,					\
    studio	 TEXT NOT NULL,						\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'DVD'			\
);									\
									\
CREATE TABLE dvd_copy_info						\
(									\
    copy_number	 INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    item_oid	 BIGINT NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    PRIMARY KEY(item_oid, copyid),					\
    FOREIGN KEY(item_oid) REFERENCES dvd(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TRIGGER dvd_purge_trigger AFTER DELETE ON dvd			\
FOR EACH row								\
BEGIN									\
    DELETE FROM dvd_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	        type = old.type;					\
END;									\
									\
CREATE TABLE grey_literature						\
(									\
    author		TEXT NOT NULL,					\
    client		TEXT,						\
    document_code_a	TEXT NOT NULL,					\
    document_code_b TEXT NOT NULL,					\
    document_date	TEXT NOT NULL,					\
    document_id	TEXT NOT NULL PRIMARY KEY,				\
    document_status TEXT,						\
    document_title	TEXT NOT NULL,					\
    document_type	TEXT NOT NULL,					\
    job_number	TEXT NOT NULL,						\
    location	TEXT,							\
    myoid		BIGINT UNIQUE,					\
    notes		TEXT,						\
    type		VARCHAR(16) NOT NULL DEFAULT 'Grey Literature'	\
);									\
									\
CREATE TABLE journal							\
(									\
    back_cover	 BYTEA,							\
    callnumber	 VARCHAR(64),						\
    category	 TEXT NOT NULL,						\
    description	 TEXT NOT NULL,						\
    deweynumber	 VARCHAR(64),						\
    front_cover	 BYTEA,							\
    id		 VARCHAR(32),						\
    issueno	 INTEGER NOT NULL DEFAULT 0,				\
    issuevolume	 INTEGER NOT NULL DEFAULT 0,				\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    lccontrolnumber	 VARCHAR(64),					\
    location	 TEXT NOT NULL,						\
    marc_tags    TEXT,							\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    pdate	 VARCHAR(32) NOT NULL,					\
    place	 TEXT NOT NULL,						\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    publisher	 TEXT NOT NULL,						\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'Journal',		\
    UNIQUE(id, issueno, issuevolume)					\
);									\
									\
CREATE TABLE journal_copy_info						\
(									\
    copy_number	 INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    item_oid	 BIGINT NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    PRIMARY KEY(item_oid, copyid),					\
    FOREIGN KEY(item_oid) REFERENCES journal(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TABLE journal_files						\
(									\
    description	TEXT,							\
    file	BYTEA NOT NULL,						\
    file_digest	TEXT NOT NULL,						\
    file_name   TEXT NOT NULL,						\
    item_oid	BIGINT NOT NULL,					\
    myoid	BIGINT NOT NULL,					\
    FOREIGN KEY(item_oid) REFERENCES journal(myoid) ON DELETE CASCADE,	\
    PRIMARY KEY(file_digest, item_oid)					\
);									\
									\
CREATE TRIGGER journal_purge_trigger AFTER DELETE ON journal		\
FOR EACH row								\
BEGIN									\
    DELETE FROM journal_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
                type = old.type;					\
END;									\
									\
CREATE TABLE magazine							\
(									\
    back_cover	 BYTEA,							\
    callnumber	 VARCHAR(64),						\
    category	 TEXT NOT NULL,						\
    description	 TEXT NOT NULL,						\
    deweynumber	 VARCHAR(64),						\
    front_cover	 BYTEA,							\
    id		 VARCHAR(32),						\
    issueno	 INTEGER NOT NULL DEFAULT 0,				\
    issuevolume	 INTEGER NOT NULL DEFAULT 0,				\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    lccontrolnumber	 VARCHAR(64),					\
    location	 TEXT NOT NULL,						\
    marc_tags    TEXT,							\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    pdate	 VARCHAR(32) NOT NULL,					\
    place	 TEXT NOT NULL,						\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    publisher	 TEXT NOT NULL,						\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'Magazine',		\
    UNIQUE(id, issuevolume, issueno)					\
);									\
									\
CREATE TABLE magazine_copy_info						\
(									\
    copy_number	 INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    item_oid	 BIGINT NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    PRIMARY KEY(item_oid, copyid),				        \
    FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE	\
);									\
									\
CREATE TABLE magazine_files						\
(									\
    description	TEXT,							\
    file	BYTEA NOT NULL,						\
    file_digest	TEXT NOT NULL,						\
    file_name   TEXT NOT NULL,						\
    item_oid	BIGINT NOT NULL,					\
    myoid	BIGINT NOT NULL,					\
    FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE CASCADE,	\
    PRIMARY KEY(file_digest, item_oid)					\
);									\
									\
CREATE TRIGGER magazine_purge_trigger AFTER DELETE ON magazine		\
FOR EACH row								\
BEGIN									\
    DELETE FROM magazine_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
	        type = old.type;					\
END;									\
									\
CREATE TABLE photograph_collection					\
(									\
    about	 TEXT,							\
    id		 TEXT PRIMARY KEY NOT NULL,				\
    image	 BYTEA,							\
    image_scaled BYTEA,							\
    location     TEXT NOT NULL,						\
    myoid	 BIGINT NOT NULL,					\
    notes	 TEXT,							\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(32) NOT NULL DEFAULT 'Photograph Collection'	\
);									\
									\
CREATE TABLE photograph							\
(									\
    callnumber		  VARCHAR(64),					\
    collection_oid	  BIGINT NOT NULL,				\
    copyright		  TEXT NOT NULL,				\
    creators		  TEXT NOT NULL,				\
    format		  TEXT,						\
    id                    TEXT NOT NULL,				\
    image		  BYTEA,					\
    image_scaled	  BYTEA,					\
    medium		  TEXT NOT NULL,				\
    myoid		  BIGINT NOT NULL,				\
    notes		  TEXT,						\
    other_number	  TEXT,						\
    pdate		  VARCHAR(32) NOT NULL,				\
    quantity		  INTEGER NOT NULL DEFAULT 1,			\
    reproduction_number   TEXT NOT NULL,				\
    subjects		  TEXT,						\
    title		  TEXT NOT NULL,				\
    PRIMARY KEY(id, collection_oid),					\
    FOREIGN KEY(collection_oid) REFERENCES				\
                                photograph_collection(myoid) ON		\
				DELETE CASCADE				\
);									\
									\
CREATE TABLE videogame							\
(									\
    back_cover	 BYTEA,							\
    description  TEXT NOT NULL,						\
    developer	 TEXT NOT NULL,						\
    front_cover  BYTEA,							\
    genre	 TEXT NOT NULL,						\
    id		 VARCHAR(32) NOT NULL PRIMARY KEY,			\
    keyword      TEXT,							\
    language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',		\
    location	 TEXT NOT NULL,						\
    monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',	\
    myoid	 BIGINT NOT NULL,					\
    place	 TEXT NOT NULL,						\
    price	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,			\
    publisher	 TEXT NOT NULL,						\
    quantity	 INTEGER NOT NULL DEFAULT 1,				\
    rdate	 VARCHAR(32) NOT NULL,					\
    title	 TEXT NOT NULL,						\
    type	 VARCHAR(16) NOT NULL DEFAULT 'Video Game',		\
    vgmode	 VARCHAR(16) NOT NULL DEFAULT 'Multiplayer',		\
    vgplatform	 VARCHAR(64) NOT NULL,					\
    vgrating	 VARCHAR(64) NOT NULL					\
);									\
									\
CREATE TABLE videogame_copy_info					\
(									\
    copy_number INTEGER NOT NULL DEFAULT 1,				\
    copyid	 VARCHAR(64) NOT NULL,					\
    item_oid	 BIGINT NOT NULL,					\
    myoid	 BIGINT NOT NULL,					\
    PRIMARY KEY(item_oid, copyid),				        \
    FOREIGN KEY(item_oid) REFERENCES videogame(myoid) ON		\
                          DELETE CASCADE				\
);									\
									\
CREATE TRIGGER videogame_purge_trigger AFTER DELETE ON videogame	\
FOR EACH row								\
BEGIN									\
    DELETE FROM videogame_copy_info WHERE item_oid = old.myoid;		\
    DELETE FROM member_history WHERE item_oid = old.myoid AND		\
      	        type = old.type;					\
END;									\
									\
CREATE TABLE item_borrower						\
(									\
    copy_number	  INTEGER NOT NULL DEFAULT 1,				\
    copyid	  VARCHAR(64) NOT NULL,					\
    duedate	  VARCHAR(32) NOT NULL,					\
    item_oid	  BIGINT NOT NULL,					\
    memberid	  VARCHAR(16) NOT NULL,					\
    myoid	  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,		\
    reserved_by	  VARCHAR(128) NOT NULL,				\
    reserved_date VARCHAR(32) NOT NULL,					\
    type	  VARCHAR(16) NOT NULL,					\
    FOREIGN KEY(memberid) REFERENCES member ON DELETE RESTRICT		\
);									\
									\
CREATE TABLE member							\
(									\
    city	     VARCHAR(256) NOT NULL,				\
    comments         TEXT,						\
    dob		     VARCHAR(32) NOT NULL,				\
    email	     VARCHAR(128),					\
    expiration_date  VARCHAR(32) NOT NULL,			        \
    first_name	     VARCHAR(128) NOT NULL,				\
    general_registration_number TEXT,					\
    last_name	     VARCHAR(128) NOT NULL,				\
    memberclass      TEXT,						\
    memberid	     VARCHAR(16) NOT NULL PRIMARY KEY DEFAULT 1,	\
    membersince	     VARCHAR(32) NOT NULL,				\
    middle_init	     VARCHAR(1),					\
    overdue_fees     NUMERIC(10, 2) NOT NULL DEFAULT 0.00,		\
    sex		     VARCHAR(32) NOT NULL DEFAULT 'Private',		\
    state_abbr	     VARCHAR(16) NOT NULL DEFAULT 'N/A',		\
    street	     VARCHAR(256) NOT NULL,				\
    telephone_num    VARCHAR(32),					\
    zip		     VARCHAR(16) NOT NULL DEFAULT 'N/A'			\
);									\
									\
CREATE TABLE member_history						\
(									\
    memberid	  VARCHAR(16) NOT NULL,					\
    item_oid	  BIGINT NOT NULL,					\
    copyid	  VARCHAR(64) NOT NULL,					\
    reserved_date VARCHAR(32) NOT NULL,					\
    duedate	  VARCHAR(32) NOT NULL,					\
    returned_date VARCHAR(32) NOT NULL,					\
    myoid	  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,		\
    reserved_by	  VARCHAR(128) NOT NULL,				\
    type	  VARCHAR(16) NOT NULL,					\
    FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE \
);									\
									\
CREATE TRIGGER member_history_trigger AFTER DELETE ON member		\
FOR EACH row								\
BEGIN									\
    DELETE FROM member_history WHERE memberid = old.memberid;		\
END;									\
									\
CREATE VIEW item_borrower_vw AS						\
SELECT	 copy_number,							\
	 copyid,							\
	 duedate,							\
	 memberid,							\
	 myoid,								\
	 reserved_date,							\
	 type,								\
         item_oid							\
FROM	 item_borrower;						        \
									\
CREATE TABLE book_binding_types						\
(									\
    binding_type     TEXT NOT NULL PRIMARY KEY				\
);									\
									\
CREATE TABLE locations				                        \
(									\
    location	 TEXT NOT NULL,						\
    type	 VARCHAR(32) NOT NULL,					\
    PRIMARY KEY(location, type)						\
);      								\
									\
CREATE TABLE monetary_units						\
(									\
    monetary_unit	 TEXT NOT NULL PRIMARY KEY                      \
);                                                                      \
									\
CREATE TABLE languages							\
(									\
    language	 TEXT NOT NULL PRIMARY KEY				\
);      								\
									\
CREATE TABLE cd_formats							\
(									\
    cd_format	 TEXT NOT NULL PRIMARY KEY				\
);                                                                      \
									\
CREATE TABLE dvd_ratings						\
(									\
    dvd_rating	 TEXT NOT NULL PRIMARY KEY				\
);                                                                      \
									\
CREATE TABLE dvd_aspect_ratios						\
(									\
    dvd_aspect_ratio	 TEXT NOT NULL PRIMARY KEY			\
);                                                                      \
									\
CREATE TABLE dvd_regions						\
(									\
    dvd_region	 TEXT NOT NULL PRIMARY KEY				\
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
    videogame_rating	 TEXT NOT NULL PRIMARY KEY			\
);                                                                      \
									\
CREATE TABLE videogame_platforms					\
(									\
    videogame_platform	 TEXT NOT NULL PRIMARY KEY			\
);                                                                      \
									\
CREATE TABLE sequence							\
(									\
    value            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT		\
);									\
";
