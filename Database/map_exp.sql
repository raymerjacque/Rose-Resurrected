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



-- Dumping structure for table roseza.map_exp
DROP TABLE IF EXISTS `map_exp`;
CREATE TABLE IF NOT EXISTS `map_exp` (
  `id` int(100) NOT NULL AUTO_INCREMENT,
  `map_id` int(100) NOT NULL,
  `map_name` varchar(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=26 DEFAULT CHARSET=latin1;

-- Dumping data for table roseza.map_exp: 25 rows
/*!40000 ALTER TABLE `map_exp` DISABLE KEYS */;
INSERT INTO `map_exp` (`id`, `map_id`, `map_name`) VALUES
	(1, 1, 'Canyon City of Zant'),
	(2, 2, 'City of Junon Polis'),
	(3, 21, 'Valley of Luxem Tower'),
	(4, 22, 'Adventurer\'s Plains'),
	(5, 24, 'El Verloon Desert'),
	(6, 25, 'Anima Lake'),
	(7, 26, 'Forest of Wisdom'),
	(8, 27, 'Kenji Beach'),
	(9, 28, 'Gorge of Silence'),
	(10, 29, 'Desert of the Dead'),
	(11, 42, 'Oblivion Temple (B2)'),
	(12, 43, 'Oblivion Temple (B3)'),
	(13, 81, 'Wasteland Ruins Path'),
	(14, 52, 'Mana Snowfields'),
	(15, 53, 'Arumic Valley'),
	(16, 65, 'Sikuku Underground Prison'),
	(17, 62, 'Shady Jungle'),
	(18, 63, 'Forest of Wandering'),
	(19, 64, 'Marsh of Ghosts'),
	(20, 61, 'Xita Refuge'),
	(21, 66, 'Sikuku Ruins'),
	(22, 78, 'The Wasteland'),
	(23, 79, 'The Wasteland'),
	(24, 44, 'Oblivion Temple (B4)'),
	(25, 81, 'Wasteland Ruins Path');
/*!40000 ALTER TABLE `map_exp` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
