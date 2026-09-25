

	ALTER TABLE `characters` ADD COLUMN IF NOT EXISTS `groupid` INT UNSIGNED NOT NULL DEFAULT '0' AFTER `timergraydrop`;
	ALTER TABLE `list_npcs_special` ADD COLUMN IF NOT EXISTS `buffbot` TINYINT(4) UNSIGNED NOT NULL DEFAULT '0' AFTER `whatisit`;
	ALTER TABLE `list_npcs_special` ADD COLUMN IF NOT EXISTS `buffpower` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `buffbot`;
	
	INSERT IGNORE INTO `list_npcs_special` (`id`, `name`, `type`, `map`, `dir`, `x`, `y`, `isactive`, `whatisit`, `buffbot`, `buffpower`) VALUES ('22', 'Buff Fairy', '1030', '1', '275.986', '5246', '5283', '1', 'BUFFBOT', '1', '300');
	
	ALTER TABLE `items` ADD COLUMN IF NOT EXISTS `restriction` INT(11) NOT NULL DEFAULT '0' AFTER `refine`;
	ALTER TABLE `items` ADD COLUMN IF NOT EXISTS `usetype` INT(11) NOT NULL DEFAULT '0' AFTER `restriction`;
	ALTER TABLE `list_config` ADD COLUMN IF NOT EXISTS `max_refine` INT(11) NOT NULL DEFAULT '9' AFTER `item_drop_rate`;
	ALTER TABLE `list_config` ADD COLUMN IF NOT EXISTS `no_trade_unequip` INT(11) NOT NULL DEFAULT '9' AFTER `max_refine`;
	
