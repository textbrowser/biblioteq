/* Release 3.22 */

ALTER TABLE book ADD type VARCHAR(16) NOT NULL DEFAULT 'Book';
ALTER TABLE cd ADD type VARCHAR(16) NOT NULL DEFAULT 'CD';
ALTER TABLE dvd ADD type VARCHAR(16) NOT NULL DEFAULT 'DVD';
ALTER TABLE videogame ADD type VARCHAR(16) NOT NULL DEFAULT 'Video Game';

/* Releases 3.23, 3.23.1 */

CREATE TABLE member_history
(
	memberid	 VARCHAR(16) NOT NULL,
	item_oid	 INTEGER NOT NULL,
	copyid		 VARCHAR(64) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	returned_date	 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	reserved_by	 VARCHAR(128) NOT NULL,
	type		 VARCHAR(16) NOT NULL,
	item_id		 VARCHAR(32) NOT NULL,
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
) engine = InnoDB;

GRANT DELETE, INSERT, SELECT, UPDATE ON xbook_db.* TO xbook_admin@localhost IDENTIFIED BY 'xbook_admin';

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

DROP USER xbook_admin@localhost;
CREATE USER xbook_admin@'%' IDENTIFIED BY 'xbook_admin';
GRANT DELETE, SELECT, UPDATE ON xbook_db.* TO xbook_admin@'%' IDENTIFIED BY 'xbook_admin';
GRANT CREATE USER ON xbook_db.* TO xbook_admin@'%' IDENTIFIED BY 'xbook_admin';
DROP USER xbook@localhost;
