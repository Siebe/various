<?php include 'back_header.php';?>
<h3>Details Formulier voor "<?php echo $work->title ?>"</h3>
<?php if (isset($message)) echo '<p class="message">'.$message.'</p>' ?>
<p> Voeg hier een extra afbeelding toe, zie onder voor een lijst aan extra afbeeldingen </p>
<?php FormHelper::form('details_form', $form_fields, null, $validation_error); ?>

<div class="table" id="item-list">
    <div class="table-row table-header">
        <div class="table-cell icon">
            Afbeelding
        </div>
        <div class="table-cell">
            Toegevoegd op
        </div>
        <div class="table-cell">
            Acties:
        </div>
        <div class="table-cell">
        </div>

    </div>
<?php 
$i=1;
foreach ($all_details as $details) {
    echo '  <div class="table-row'.(($i++ & 1 == 1) ? ' odd' : '').'">';
    echo '      <div class="table-cell icon">';
    echo '          <a href=/afbeelding/'.$work->url_title.'/'.$details->id.' target="_blank"><img src="'.ImageHelper::icon($details->image).'"></a>';
    echo '      </div><div class="table-cell created_at">';
    echo '     '.strftime('%c', strtotime($details->created_at));
    echo '      </div><div class="table-cell updated_at">';
    echo '          <a href="/backend/details/touch/'.$details->id.'">Bovenaan zetten</a>';
    echo '      </div><div class="table-cell delete">';
    echo '          <a href="/backend/details/delete/'.$details->id.'" onclick="return delete_warning(\''.$details->title.'\')">Verwijderen</a>';
    echo '      </div>';    
    echo '  </div>';
}?>
</div><br>

<a href ="/backend/werk">
    <div class ="button action-button">Terug naar werk menu</div>
</a>
<a href ="/backend/werk/edit/<?php echo $work->id ?>">
    <div class ="button action-button">Terug naar werk</div>
</a>


<?php include 'footer.php';?>

<script>
    function delete_warning(title) {
        return confirm('Werk "'+title+'" wordt verwijderd, zeker weten?');
    }
</script>