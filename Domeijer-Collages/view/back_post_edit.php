<?php include 'back_header.php';?>
<h3>Nieuws Formulier</h3>
<?php if (isset($message)) echo '<p class="message">'.$message.'</p>' ?>

<?php FormHelper::form('post_form', $form_fields, $post, $validation_error); ?>


<a href ="/backend/nieuws">
    <div class ="button action-button">Terug naar nieuws menu</div>
</a>

<?php include 'footer.php';?>


<script>
    window.addEventListener('load',  function() { 
        tinymce.init({selector:'textarea', plugins: "link wordcount image", language : 'nl'});
    });
</script>