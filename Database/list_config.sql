-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               10.4.28-MariaDB - mariadb.org binary distribution
-- Server OS:                    Win64
-- HeidiSQL Version:             12.4.0.6659
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

-- Dumping structure for table roseza.list_config
DROP TABLE IF EXISTS `list_config`;
CREATE TABLE IF NOT EXISTS `list_config` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `exp_rate` int(11) NOT NULL,
  `kill_on_fail` tinyint(1) NOT NULL DEFAULT 0,
  `drop_rate` int(11) NOT NULL,
  `zuly_rate` int(11) NOT NULL,
  `blue_chance` int(11) NOT NULL,
  `slot_chance` int(11) NOT NULL,
  `stat_chance` int(11) NOT NULL,
  `refine_chance` int(11) NOT NULL,
  `rare_refine` int(11) NOT NULL DEFAULT 0,
  `conf` varchar(50) NOT NULL,
  `player_damage` int(11) NOT NULL,
  `monster_damage` int(11) NOT NULL,
  `player_acc` int(11) NOT NULL,
  `monster_acc` int(11) NOT NULL,
  `pvp_acc` int(11) NOT NULL,
  `skill_damage` int(11) NOT NULL,
  `maxlevel` int(11) NOT NULL,
  `drop_type` int(11) NOT NULL,
  `savetime` int(11) NOT NULL,
  `partygap` int(11) NOT NULL,
  `maxstat` int(11) NOT NULL,
  `cfmode` int(11) NOT NULL,
  `autosave` int(11) NOT NULL,
  `mapdelay` int(11) NOT NULL,
  `visualdelay` int(11) NOT NULL,
  `worlddelay` int(11) NOT NULL,
  `fairymode` int(11) NOT NULL,
  `fairystay` int(11) NOT NULL,
  `fairywait` int(11) NOT NULL,
  `fairytestmode` int(11) NOT NULL,
  `osRoseVer` varchar(11) NOT NULL,
  `testgrid` int(11) NOT NULL,
  `jrose` int(11) NOT NULL,
  `is_pegasus` int(11) NOT NULL,
  `monmax` int(11) NOT NULL DEFAULT 10,
  `massexport` int(11) NOT NULL DEFAULT 0,
  `uwnbplayers` int(11) NOT NULL DEFAULT 0,
  `uwside` int(11) NOT NULL DEFAULT 0,
  `pc_drop_zuly` int(11) NOT NULL DEFAULT 30,
  `drop_rev` int(11) NOT NULL DEFAULT 1,
  `advertise_active` int(11) NOT NULL DEFAULT 0,
  `advertise_time` int(11) NOT NULL DEFAULT 300 COMMENT 'Works in seconds',
  `item_drop_rate` int(11) NOT NULL,
  `max_refine` int(11) NOT NULL DEFAULT 9,
  `no_trade_unequip` int(11) NOT NULL DEFAULT 9,
  `more_sockets` int(11) NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;

-- Dumping data for table roseza.list_config: 1 rows
/*!40000 ALTER TABLE `list_config` DISABLE KEYS */;
INSERT IGNORE INTO `list_config` (`id`, `exp_rate`, `kill_on_fail`, `drop_rate`, `zuly_rate`, `blue_chance`, `slot_chance`, `stat_chance`, `refine_chance`, `rare_refine`, `conf`, `player_damage`, `monster_damage`, `player_acc`, `monster_acc`, `pvp_acc`, `skill_damage`, `maxlevel`, `drop_type`, `savetime`, `partygap`, `maxstat`, `cfmode`, `autosave`, `mapdelay`, `visualdelay`, `worlddelay`, `fairymode`, `fairystay`, `fairywait`, `fairytestmode`, `osRoseVer`, `testgrid`, `jrose`, `is_pegasus`, `monmax`, `massexport`, `uwnbplayers`, `uwside`, `pc_drop_zuly`, `drop_rev`, `advertise_active`, `advertise_time`, `item_drop_rate`, `max_refine`, `no_trade_unequip`, `more_sockets`) VALUES
	(1, 1, 0, 400, 50, 5, 5, 5, 5, 0, 'default', 100, 100, 100, 100, 100, 100, 250, 12, 100, 15, 370, 1, 1, 10, 200, 700, 0, 45, 30, 0, '5003.136', 1, 0, 0, 10, 1, 2, 1, 30, 1, 1, 1200, 1, 9, 9, 1);
/*!40000 ALTER TABLE `list_config` ENABLE KEYS */;

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
