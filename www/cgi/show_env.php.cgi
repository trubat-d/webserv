<?php
ob_start();
?>
<html>
    <body>
        <h1>Env (php):</h1>
        <?php
            foreach($_ENV as $key => $value) {
              echo "<p>" . $key . "=" . $value;
            }
            header("Status: 200 OK");
        ?>
        <br><br><a href=\"/\">Return Home !</a>
    </body>
</html>
<?php
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>