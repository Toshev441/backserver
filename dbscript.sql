CREATE TABLE IF NOT EXISTS "devices" (
	"ID"	CHAR(128) NOT NULL UNIQUE,
	"name"	CHAR(50) NOT NULL,
	"description"	TEXT NOT NULL,
	"session"	TEXT NOT NULL,
	"app_ID"	CHAR(36) NOT NULL,
	"last_seen_at" CHAR(22) NOT NULL,
	PRIMARY KEY("ID")
);
CREATE TABLE IF NOT EXISTS "applications" (
	"ID"	CHAR(36) NOT NULL UNIQUE,
	"name"	char(50) NOT NULL,
	"description"	TEXT NOT NULL,
	"app_key"	TEXT NOT NULL,
	"service_ID"  CHAR(36) NOT NULL,
	PRIMARY KEY("ID")
);
CREATE TABLE IF NOT EXISTS "queue" (
	"time_insert"	CHAR(50) NOT NULL,
	"actual_period"	INTEGER NOT NULL,
	"device_ID"	CHAR(128) NOT NULL,
	"buffer"	        TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS "services" (
	"ID"	CHAR(36) NOT NULL UNIQUE,
	"name"	char(50) NOT NULL,
	"description"	TEXT NOT NULL,
	"token"	TEXT NOT NULL,
	PRIMARY KEY("ID")
);
CREATE TABLE IF NOT EXISTS "integrations" (
	"ID"	CHAR(36) NOT NULL UNIQUE,
	"name"	CHAR(50) NOT NULL,
	"app_ID"	CHAR(36) NOT NULL,
	"type"	CHAR(50) NOT NULL,
	"integration" TEXT NOT NULL,
	PRIMARY KEY("ID")
);
