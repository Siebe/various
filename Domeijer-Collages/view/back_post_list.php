<?php include 'back_header.php';?>
<h3>Nieuws Lijst</h3>
<ul class="item-list">
<div class="table" id="item-list">
    <div class="table-row table-header">
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
            Verwijderen
        </div>
    </div>
<?php 
$i=1;
foreach ($all_post as $post) {
    echo '  <div class="table-row'.(($i++ & 1 == 1) ? ' odd' : '').'">';
    echo '      <div class="table-cell title">';
    echo '          <a href=/backend/nieuws/edit/'.$post->id.">".$post->title.'</a>';
    echo '      </div><div class="table-cell public">';
    echo ($post->published ? '      Ja' : '     Nee');
    echo '      </div><div class="table-cell created_at">';
    echo '     '.strftime('%c', strtotime($post->created_at));
    echo '      </div><div class="table-cell updated_at">';
    echo '     '.strftime('%c', strtotime($post->updated_at));
    echo '      </div><div class="table-cell delete">';
    echo '          <a href="/backend/nieuws/delete/'.$post->id.'" onclick="return delete_warning(\''.$post->title.'\')">Verwijderen</a>';
    echo '      </div>';    
    echo '  </div>';
}?>
</div><br>

<a href ="/backend/nieuws/edit">
    <div class ="button action-button">Nieuw item toevoegen</div>
</a>

<a href ="/backend">
    <div class ="button action-button">Terug naar hoofdmenu</div>
</a>

<script>
    function delete_warning(title) {
        return confirm('Werk "'+title+'" wordt verwijderd, zeker weten?');
    }
</script>



<?php include 'footer.php';?>
