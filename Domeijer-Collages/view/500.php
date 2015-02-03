<?php 
http_response_code (500 );
header( $_SERVER["SERVER_PROTOCOL"] . ' 500 Server Error'); ?>
<?php include ($backend ? 'back_header.php' : 'header.php');?>
<p>Fout in de server!</p> 
<?php if (isset($error)) echo '<p class="alert">'.$error.'<p>' ?>
<?php include ('footer.php');?>