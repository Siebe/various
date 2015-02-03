<?php include 'back_header.php';?>
<h3>Werk Formulier</h3>
<?php if (isset($message)) echo '<p class="message">'.$message.'</p>' ?>

<?php FormHelper::form('work_form', $form_fields, $work, $validation_error); ?>

<?php if ($work_is_stored) : ?>
<a href ="/backend/details/edit/<?php echo $work->id ?>">
    <div class ="button action-button">Extra afbeeldingen</div>
</a>

<a href ="/backend/werk/edit">
    <div class ="button action-button">Nieuw werk toevoegen</div>
</a>
<?php endif;?>

<a href ="/backend/werk">
    <div class ="button action-button">Terug naar werk menu</div>
</a>

<?php include 'footer.php';?>
