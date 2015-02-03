<?php include 'back_header.php';?>



<form id='login' action='/backend' method='post' accept-charset='UTF-8'>

<fieldset >
<legend>Login</legend>
<input type='hidden' name='submitted' id='submitted' value='1'/>

 <?php if (isset($message))echo '<p class="alert">'.$message.'</p><br>' ?>

<label for='username' >Naam:</label>
<input type='text' name='username' id='username'  maxlength="50" />
 
<label for='password' >Wachtwoord:</label>
<input type='password' name='password' id='password' maxlength="50" />
 
<input type='submit' name='Submit' value='Inloggen' />
 
</fieldset>
<?php if (isset($userinfo)) echo $userinfo ?>
<?php include 'footer.php';?>
