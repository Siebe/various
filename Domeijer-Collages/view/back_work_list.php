<?php include 'back_header.php';?>
<h3>Werk Lijst</h3>
<p>Klik op de afbeelding of titel om te bewerken, of klik op "Extra Afbeeldingen" voor meer details.</p>

<div class="table" id="item-list">
    <div class="table-row table-header">
        <div class="table-cell icon">
            Afbeelding
        </div>
        <div class="table-cell">
            Titel
        </div>
        <div class="table-cell">
            Publiek
        </div>
        <div class="table-cell">
            Toegevoegd op
        </div>
        <div class="table-cell">
            Aangepast op
        </div>
        <div class="table-cell">
            Acties:
        </div>
        <div class="table-cell">
        </div>
        <div class="table-cell">
        </div>
    </div>
<?php 
$i=1;
foreach ($all_work as $work) {
    echo '  <div class="table-row'.(($i++ & 1 == 1) ? ' odd' : '').'">';
    echo '      <div class="table-cell icon">';
    echo '          <a href=/backend/werk/edit/'.$work->id.'><img src="'.ImageHelper::icon($work->image).'"></a>';
    echo '      </div><div class="table-cell title">';
    echo '          <a href=/backend/werk/edit/'.$work->id.">".$work->title.'</a>';
    echo '      </div><div class="table-cell public">';
    echo ($work->published ? '      Ja' : '     Nee');
    echo '      </div><div class="table-cell created_at">';
    echo '     '.strftime('%c', strtotime($work->created_at));
    echo '      </div><div class="table-cell updated_at">';
    echo '     '.strftime('%c', strtotime($work->updated_at));
    echo '      </div><div class="table-cell details">';
    echo '          <a href="/backend/details/edit/'.$work->id.'">Extra Afbeeldingen('.sizeof($work->xownDetailsList).')</a>';
    echo '      </div><div class="table-cell touch">';
    echo '          <a href="/backend/werk/touch/'.$work->id.'">Bovenaan zetten</a>';
    echo '      </div><div class="table-cell delete">';
    echo '          <a href="/backend/werk/delete/'.$work->id.'" onclick="return delete_warning(\''.$work->title.'\')">Verwijderen</a>';
    echo '      </div>';    
    echo '  </div>';
}?>
</div><br>

<a href ="/backend/werk/edit">
    <div class ="button action-button">Nieuw werk toevoegen</div>
</a>

<a href ="/backend">
    <div class ="button action-button">Terug naar hoofdmenu</div>
</a>

<?php include 'footer.php';?>

<script>
    function delete_warning(title) {
        return confirm('Werk "'+title+'" wordt verwijderd, zeker weten?');
    }
</script>