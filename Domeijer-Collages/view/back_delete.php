<?php include 'back_header.php';?>
<h3>Verwijderen <?php echo $delete_model ?></h3>

<p><b>"<?php echo $delete_name ?>"</b></p>
<?php if (isset($image_error)) echo '<p class="alert">'.$image_error.'</p>' ?>
<p><?php echo $message ?></p>
   
<a href ="<?php echo $redirect?>">
    <div class ="button action-button">Naar vorig menu</div>
</a>
    
<a href ="/backend">
    <div class ="button action-button">Terug naar hoofdmenu</div>
</a>

<?php include 'footer.php';?>