

	ALTER TABLE `characters` ADD COLUMN IF NOT EXISTS `groupid` INT UNSIGNED NOT NULL DEFAULT '0' AFTER `timergraydrop`;
	ALTER TABLE `list_npcs_special` ADD COLUMN IF NOT EXISTS `buffbot` TINYINT(4) UNSIGNED NOT NULL DEFAULT '0' AFTER `whatisit`;
	ALTER TABLE `list_npcs_special` ADD COLUMN IF NOT EXISTS `buffpower` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `buffbot`;
	
	INSERT IGNORE INTO `list_npcs_special` (`id`, `name`, `type`, `map`, `dir`, `x`, `y`, `isactive`, `whatisit`, `buffbot`, `buffpower`) VALUES ('22', 'Buff Fairy', '1030', '1', '275.986', '5246', '5283', '1', 'BUFFBOT', '1', '300');
	
	ALTER TABLE `items` ADD COLUMN IF NOT EXISTS `restriction` INT(11) NOT NULL DEFAULT '0' AFTER `refine`;
	ALTER TABLE `items` ADD COLUMN IF NOT EXISTS `usetype` INT(11) NOT NULL DEFAULT '0' AFTER `restriction`;
	ALTER TABLE `list_config` ADD COLUMN IF NOT EXISTS `max_refine` INT(11) NOT NULL DEFAULT '9' AFTER `item_drop_rate`;
	ALTER TABLE `list_config` ADD COLUMN IF NOT EXISTS `no_trade_unequip` INT(11) NOT NULL DEFAULT '9' AFTER `max_refine`;
	
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `is_read` TINYINT(1) NOT NULL DEFAULT 0;
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `zuly` BIGINT UNSIGNED NOT NULL DEFAULT 0;
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `item_head` INT UNSIGNED NOT NULL DEFAULT 0;
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `item_data` INT UNSIGNED NOT NULL DEFAULT 0;
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `item_name` VARCHAR(64) NOT NULL DEFAULT '';
	ALTER TABLE `mail_list` ADD COLUMN IF NOT EXISTS `is_claimed` TINYINT(1) NOT NULL DEFAULT 0;

	ALTER TABLE `list_clan` ADD COLUMN IF NOT EXISTS `money` BIGINT UNSIGNED NOT NULL DEFAULT 0;
	ALTER TABLE `list_clan` ADD COLUMN IF NOT EXISTS `skills` VARCHAR(500) NOT NULL DEFAULT '';

	CREATE TABLE IF NOT EXISTS `clan_storage` (
	  `clanid` int(11) NOT NULL,
	  `slotnum` int(11) NOT NULL,
	  `itemnum` int(11) DEFAULT 0,
	  `itemtype` int(11) DEFAULT 0,
	  `count` int(11) DEFAULT 0,
	  `lifespan` int(11) DEFAULT 100,
	  `itemdurability` int(11) DEFAULT 100,
	  `itemstats` int(11) DEFAULT 0,
	  `itemappraisal` int(11) DEFAULT 0,
	  `itemgem` int(11) DEFAULT 0,
	  `itemrefine` int(11) DEFAULT 0,
	  `itemisappraised` int(11) DEFAULT 0,
	  `restriction` int(11) DEFAULT 0,
	  `usetype` int(11) DEFAULT 0,
	  PRIMARY KEY (`clanid`, `slotnum`)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1;

