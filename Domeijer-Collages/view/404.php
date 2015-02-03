<?php 
http_response_code (404);
header( $_SERVER["SERVER_PROTOCOL"] . ' 404 Not Found'); ?>
<?php include ($backend ? 'back_header.php' : 'header.php');?>
Pagina niet gevonden!
<?php include ('footer.php');?>
