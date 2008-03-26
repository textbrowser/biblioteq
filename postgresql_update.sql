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
