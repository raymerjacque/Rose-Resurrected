<?php
/**
 * RosE Resurrected — Web Portal API Engine
 * Standard PHP PDO database bridge for rose_resurrected database.
 */

header('Content-Type: application/json; charset=utf-8');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');

if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    exit(0);
}

session_start();

$db_host = 'localhost';
$db_user = 'root';
$db_pass = 'sproggins';
$db_name = 'rose_resurrected';
$db_port = 3306;

try {
    $pdo = new PDO("mysql:host={$db_host};port={$db_port};dbname={$db_name};charset=utf8mb4", $db_user, $db_pass, [
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
        PDO::ATTR_EMULATE_PREPARES => false,
    ]);
} catch (PDOException $e) {
    echo json_encode([
        'status' => 'error',
        'message' => 'Database connection failure: ' . $e->getMessage()
    ]);
    exit(0);
}

function checkPort($host, $port, $timeout = 1) {
    $fp = @fsockopen($host, $port, $errno, $errstr, $timeout);
    if ($fp) {
        fclose($fp);
        return true;
    }
    return false;
}

$action = $_GET['action'] ?? ($_POST['action'] ?? 'status');

switch ($action) {
    case 'status':
        $login_status = checkPort('127.0.0.1', 29000);
        $char_status  = checkPort('127.0.0.1', 29100);
        $world_status = checkPort('127.0.0.1', 29200);

        $acc_stmt = $pdo->query("SELECT COUNT(*) AS total FROM accounts");
        $total_accounts = (int)$acc_stmt->fetch()['total'];

        $char_stmt = $pdo->query("SELECT COUNT(*) AS total FROM characters");
        $total_characters = (int)$char_stmt->fetch()['total'];

        $online_stmt = $pdo->query("SELECT COUNT(*) AS total FROM accounts WHERE online=1");
        $online_players = (int)$online_stmt->fetch()['total'];

        echo json_encode([
            'status' => 'success',
            'servers' => [
                'login' => $login_status,
                'char'  => $char_status,
                'world' => $world_status
            ],
            'stats' => [
                'total_accounts'   => $total_accounts,
                'total_characters' => $total_characters,
                'online_players'   => $online_players,
                'exp_rate'         => '3x',
                'drop_rate'        => '3x',
                'zuly_rate'        => '3x'
            ]
        ]);
        break;

    case 'register':
        if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
            echo json_encode(['status' => 'error', 'message' => 'Invalid request method.']);
            break;
        }

        $username = trim($_POST['username'] ?? '');
        $password = trim($_POST['password'] ?? '');
        $confirm  = trim($_POST['confirm_password'] ?? '');
        $email    = trim($_POST['email'] ?? '');

        if (empty($username) || empty($password) || empty($email)) {
            echo json_encode(['status' => 'error', 'message' => 'All fields (Username, Password, Email) are required.']);
            break;
        }

        if (strlen($username) < 3 || strlen($username) > 16) {
            echo json_encode(['status' => 'error', 'message' => 'Username must be between 3 and 16 characters long.']);
            break;
        }

        if (!preg_match('/^[a-zA-Z0-9]+$/', $username)) {
            echo json_encode(['status' => 'error', 'message' => 'Username can only contain alphanumeric characters (A-Z, 0-9).']);
            break;
        }

        if (strlen($password) < 4) {
            echo json_encode(['status' => 'error', 'message' => 'Password must be at least 4 characters long.']);
            break;
        }

        if ($confirm !== '' && $password !== $confirm) {
            echo json_encode(['status' => 'error', 'message' => 'Password and Confirm Password do not match.']);
            break;
        }

        if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
            echo json_encode(['status' => 'error', 'message' => 'Please provide a valid email address.']);
            break;
        }

        // Check for existing account
        $stmt = $pdo->prepare("SELECT id FROM accounts WHERE username = :username LIMIT 1");
        $stmt->execute([':username' => $username]);
        if ($stmt->fetch()) {
            echo json_encode(['status' => 'error', 'message' => 'Username is already taken. Please choose another username.']);
            break;
        }

        // MD5 hash password for osROSE server format
        $md5_pass = md5($password);

        $insert_stmt = $pdo->prepare("INSERT INTO accounts (username, password, email, accesslevel, active, online) VALUES (:username, :password, :email, 100, 1, 0)");
        $insert_stmt->execute([
            ':username' => $username,
            ':password' => $md5_pass,
            ':email'    => $email
        ]);

        echo json_encode([
            'status' => 'success',
            'message' => 'Account successfully registered! You can now log in.'
        ]);
        break;

    case 'login':
        if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
            echo json_encode(['status' => 'error', 'message' => 'Invalid request method.']);
            break;
        }

        $username = trim($_POST['username'] ?? '');
        $password = trim($_POST['password'] ?? '');

        if (empty($username) || empty($password)) {
            echo json_encode(['status' => 'error', 'message' => 'Please enter both username and password.']);
            break;
        }

        $md5_pass = md5($password);

        $stmt = $pdo->prepare("SELECT id, username, accesslevel, email, active, platinum, online FROM accounts WHERE username = :username AND password = :password LIMIT 1");
        $stmt->execute([
            ':username' => $username,
            ':password' => $md5_pass
        ]);

        $account = $stmt->fetch();

        if (!$account) {
            echo json_encode(['status' => 'error', 'message' => 'Invalid username or password.']);
            break;
        }

        if ((int)$account['active'] === 0) {
            echo json_encode(['status' => 'error', 'message' => 'This account has been deactivated or suspended.']);
            break;
        }

        $_SESSION['account_id']   = (int)$account['id'];
        $_SESSION['username']     = $account['username'];
        $_SESSION['accesslevel']  = (int)$account['accesslevel'];

        // Get characters
        $char_stmt = $pdo->prepare("SELECT char_name, level, classid, sex, face, hairStyle, zuly, exp, deletetime FROM characters WHERE account_name = :username ORDER BY level DESC");
        $char_stmt->execute([':username' => $username]);
        $characters = $char_stmt->fetchAll();

        echo json_encode([
            'status' => 'success',
            'message' => 'Login successful!',
            'account' => $account,
            'characters' => $characters
        ]);
        break;

    case 'get_account':
        if (empty($_SESSION['username'])) {
            echo json_encode(['status' => 'unauthenticated']);
            break;
        }

        $username = $_SESSION['username'];
        $stmt = $pdo->prepare("SELECT id, username, accesslevel, email, active, platinum, online FROM accounts WHERE username = :username LIMIT 1");
        $stmt->execute([':username' => $username]);
        $account = $stmt->fetch();

        if (!$account) {
            session_destroy();
            echo json_encode(['status' => 'unauthenticated']);
            break;
        }

        $char_stmt = $pdo->prepare("SELECT char_name, level, classid, sex, face, hairStyle, zuly, exp, deletetime FROM characters WHERE account_name = :username ORDER BY level DESC");
        $char_stmt->execute([':username' => $username]);
        $characters = $char_stmt->fetchAll();

        echo json_encode([
            'status' => 'success',
            'account' => $account,
            'characters' => $characters
        ]);
        break;

    case 'change_password':
        if (empty($_SESSION['username'])) {
            echo json_encode(['status' => 'error', 'message' => 'You must be logged in to change your password.']);
            break;
        }

        $old_pass = trim($_POST['old_password'] ?? '');
        $new_pass = trim($_POST['new_password'] ?? '');

        if (empty($old_pass) || empty($new_pass)) {
            echo json_encode(['status' => 'error', 'message' => 'Please fill in both old and new password fields.']);
            break;
        }

        if (strlen($new_pass) < 4) {
            echo json_encode(['status' => 'error', 'message' => 'New password must be at least 4 characters long.']);
            break;
        }

        $username = $_SESSION['username'];
        $stmt = $pdo->prepare("SELECT id FROM accounts WHERE username = :username AND password = :password LIMIT 1");
        $stmt->execute([
            ':username' => $username,
            ':password' => md5($old_pass)
        ]);

        if (!$stmt->fetch()) {
            echo json_encode(['status' => 'error', 'message' => 'Current password is incorrect.']);
            break;
        }

        $update_stmt = $pdo->prepare("UPDATE accounts SET password = :new_pass WHERE username = :username");
        $update_stmt->execute([
            ':new_pass' => md5($new_pass),
            ':username' => $username
        ]);

        echo json_encode(['status' => 'success', 'message' => 'Password updated successfully!']);
        break;

    case 'leaderboard':
        $top_chars_stmt = $pdo->query("SELECT char_name, level, classid, sex, zuly, exp FROM characters ORDER BY level DESC, exp DESC LIMIT 20");
        $top_chars = $top_chars_stmt->fetchAll();

        $top_clans_stmt = $pdo->query("SELECT id, name, grade, cp, slogan, money FROM list_clan ORDER BY grade DESC, cp DESC LIMIT 15");
        $top_clans = $top_clans_stmt->fetchAll();

        echo json_encode([
            'status' => 'success',
            'players' => $top_chars,
            'clans' => $top_clans
        ]);
        break;

    case 'logout':
        session_destroy();
        echo json_encode(['status' => 'success', 'message' => 'Logged out successfully.']);
        break;

    default:
        echo json_encode(['status' => 'error', 'message' => 'Unknown action.']);
        break;
}
