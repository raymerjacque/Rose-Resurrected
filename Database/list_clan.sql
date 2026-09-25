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

-- Dumping structure for table roseza.list_clan
CREATE TABLE IF NOT EXISTS `list_clan` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `logo` int(11) NOT NULL DEFAULT '0',
  `back` int(11) NOT NULL DEFAULT '0',
  `name` varchar(50) DEFAULT NULL,
  `cp` int(11) NOT NULL DEFAULT '0',
  `grade` int(11) NOT NULL DEFAULT '0',
  `slogan` varchar(100) DEFAULT NULL,
  `news` varchar(200) DEFAULT NULL,
  `rankingpoints` int(11) NOT NULL DEFAULT '0',
  `siglogo` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table roseza.list_clan: ~0 rows (approximately)
DELETE FROM `list_clan`;
/*!40000 ALTER TABLE `list_clan` DISABLE KEYS */;
/*!40000 ALTER TABLE `list_clan` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
