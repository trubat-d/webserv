<?php
    ob_start();
    if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
        $filename = $_SERVER['HTTP_X_FILENAME'];
        echo $filename;
        if ($filename) {
            $filePath = "../upload/" . $filename;
            if (file_exists($filePath)) {
                unlink($filePath); // Delete the file
                echo json_encode(["message" => "File deleted successfully"]);
            } else {
                http_response_code(404);
                echo json_encode(["message" => "File not found"]);
            }
        } else {
            http_response_code(400);
            echo json_encode(["message" => "Filename is not set"]);
        }
    } else {
        http_response_code(405);
        echo json_encode(["message" => "Invalid request method"]);
    }
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>

