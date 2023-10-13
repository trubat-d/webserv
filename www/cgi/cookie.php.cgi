<?php
ob_start();
if (isset($_COOKIE['click_count'])) {
    if (isset($_POST['increment'])) {
        $count = $_COOKIE['click_count'] + 1;
    } else {
        $count = $_COOKIE['click_count'];
    }
} else {

    $count = 1;
}

setcookie('click_count', $count);

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Increment Cookie</title>
</head>
<body>

<h1>Button Click Count: <?php echo $count; ?></h1>

<form method="post">
    <input type="submit" name="increment" value="Click Me!">
</form>

</body>
</html>
<?php
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    header("Status: 200 OK");
    echo $output;
?>