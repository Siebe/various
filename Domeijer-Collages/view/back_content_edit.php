<?php include 'back_header.php';?>
<h3><?php echo $content_name ?> Formulier </h3>
<?php if (isset($message)) echo '<p class="message">'.$message.'</p>' ?>

<?php FormHelper::form('content_form', $form_fields, $content, $validation_error); ?>

<a href ="/backend">
    <div class ="button action-button">Terug naar hoofdmenu</div>
</a>

<?php include 'footer.php';?>


<script>
    window.addEventListener('load',  function() { 
        tinymce.init({selector:'textarea', plugins: "link wordcount image", language : 'nl'});
    });
</script>