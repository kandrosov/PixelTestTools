#!/bin/sh

BACKUP_PREFIX="$(hostname)"
BACKUP_ID=`date "+%Y-%m-%dT%H_%M_%S"`
BACKUP_SOURCE="/"
EXCLUDE_LIST="/soft/PixelTestTools/backup_exclude.txt"

CURRENT_USER="$( id -u )"

if [ "$CURRENT_USER" != 0 ] ; then
	echo "ERROR: backup should be run as root."
	exit 1
fi

BACKUP_DEVICE_LABEL="PixelBackup"
BACKUP_DEVICE="$( blkid -s LABEL | grep $BACKUP_DEVICE_LABEL | sed 's/:.*//' )"

if [ -z "$BACKUP_DEVICE" ] ; then
	echo "ERROR: backup device not found."
	exit 1
fi

BACKUP_MOUNT_POINT="$( mount | grep $BACKUP_DEVICE | sed 's/.* on \([^ ]*\) .*/\1/' )"

BACKUP_DESTINATION="$BACKUP_MOUNT_POINT/Backups"

if [ ! -d "$BACKUP_DESTINATION" ] ; then
	echo "ERROR: backup disk is not mounted."
	exit 1
fi

cd "$BACKUP_DESTINATION"

LAST_BACKUP="$BACKUP_PREFIX-current"
NEW_BACKUP="$BACKUP_PREFIX-$BACKUP_ID"
NEW_BACKUP_TMP="$NEW_BACKUP-incomplete"

mkdir "$NEW_BACKUP_TMP"

if [ ! -d "$NEW_BACKUP_TMP" ] ; then
	echo "ERROR: unable to create new backup directory."
	exit 1
fi

echo "Starting backup $BACKUP_ID..."

rsync -aP --delete --relative --one-file-system --numeric-ids --exclude-from="$EXCLUDE_LIST" \
      --link-dest="../$LAST_BACKUP" "$BACKUP_SOURCE" "$NEW_BACKUP_TMP/"
RESULT=$?
if [ $RESULT -ne 0 ] ; then
	echo "ERROR: something went wrong during the backup. rsync exit code = $RESULT."
	rm -rf "$NEW_BACKUP_TMP"
	exit 1
fi

mv "$NEW_BACKUP_TMP" "$NEW_BACKUP"
RESULT=$?
if [ $RESULT -ne 0 ] ; then
	echo "ERROR: unable to move incomplete backup to the final destination."
	exit 1
fi

[ -h "$LAST_BACKUP" ] && rm -f "$LAST_BACKUP"

ln -s "$NEW_BACKUP" "$LAST_BACKUP"
RESULT=$?
if [ $RESULT -ne 0 ] ; then
	echo "ERROR: unable to create a link to the current last backup."
	exit 1
fi

echo "Backup $BACKUP_ID successfully done."
