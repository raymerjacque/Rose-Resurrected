-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               10.1.16-MariaDB - mariadb.org binary distribution
-- Server OS:                    Win32
-- HeidiSQL Version:             10.1.0.5464
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- Dumping structure for table roseza.list_golist
DROP TABLE IF EXISTS `list_golist`;
CREATE TABLE IF NOT EXISTS `list_golist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mapname` varchar(50) CHARACTER SET utf8 NOT NULL,
  `lvlmin` int(11) NOT NULL DEFAULT '0',
  `access_level` int(11) NOT NULL DEFAULT '0' COMMENT 'raise to limit access',
  `map_id` int(11) NOT NULL,
  `locx` int(11) NOT NULL,
  `locy` int(11) NOT NULL,
  `isactive` int(11) NOT NULL,
  `description` varchar(50) CHARACTER SET utf8 DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;

-- Dumping data for table roseza.list_golist: ~11 rows (approximately)
/*!40000 ALTER TABLE `list_golist` DISABLE KEYS */;
INSERT INTO `list_golist` (`id`, `mapname`, `lvlmin`, `access_level`, `map_id`, `locx`, `locy`, `isactive`, `description`) VALUES
	(1, 'Adventure Plains', 0, 0, 22, 5800, 5200, 1, NULL),
	(2, 'Canyon City of Zant', 0, 0, 1, 5240, 5192, 1, NULL),
	(3, 'Junon Polis', 0, 0, 2, 5514, 5247, 1, NULL),
	(4, 'Magic City of Eucar - Luna', 50, 0, 51, 5357, 5013, 1, NULL),
	(5, 'Xita Refuge - Eldeon', 120, 0, 61, 5434, 4569, 1, NULL),
	(6, 'Training Grounds', 0, 0, 6, 5199, 5280, 1, NULL),
	(7, 'Luna Clan Field', 120, 0, 59, 5095, 5128, 1, NULL),
	(8, 'Lions Plains', 0, 0, 8, 5160, 5080, 1, NULL),
	(9, 'Desert of the Dead', 0, 0, 29, 5093, 5144, 1, NULL),
	(10, 'Sikuku Underground Prison', 160, 0, 65, 5485, 5285, 1, NULL),
	(11, 'Oro', 180, 0, 72, 5146, 5278, 1, NULL);
/*!40000 ALTER TABLE `list_golist` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
