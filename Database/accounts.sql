/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

DROP TABLE IF EXISTS `accounts`;
CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(64) DEFAULT NULL,
  `password` varchar(32) DEFAULT NULL,
  `accesslevel` int(11) DEFAULT '100',
  `lastip` varchar(15) DEFAULT '0.0.0.0',
  `lasttime` int(11) DEFAULT '0',
  `lastsvr` int(11) DEFAULT '0',
  `lastchar` varchar(64) DEFAULT NULL,
  `email` varchar(100) DEFAULT NULL,
  `nb_donation` int(11) DEFAULT '0',
  `donation` varchar(255) DEFAULT '0',
  `active` int(11) DEFAULT '1',
  `active_key` varchar(255) DEFAULT NULL,
  `zulystorage` bigint(11) DEFAULT '0',
  `platinum` tinyint(1) DEFAULT '0',
  `online` tinyint(1) DEFAULT '0',
  `login_count` int(11) DEFAULT '0',
  `isSiteLogged` tinyint(1) DEFAULT '0',
  `ktpoints` int(11) NOT NULL DEFAULT '0',
  `newpoints` int(11) NOT NULL DEFAULT '0',
  `logtime` int(11) NOT NULL DEFAULT '0',
  `totlogtime` int(11) NOT NULL DEFAULT '0',
  `can_trade` tinyint(1) NOT NULL DEFAULT '1',
  `ExpansionTime` int(11) NOT NULL DEFAULT '0',
  `counter` int(7) NOT NULL DEFAULT '0',
  `vote_points` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=88 DEFAULT CHARSET=latin1;

/*!40000 ALTER TABLE `accounts` DISABLE KEYS */;
/*!40000 ALTER TABLE `accounts` ENABLE KEYS */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
