<?php for ($i = 63; $i > 2; $i--): ?>
#define NBDL_MEMBER_IDS<?= $i ?>(NAME, SIZE, MNAME, ...) NBDL_MEMBER_ID(&NAME::MNAME, SIZE - <?= $i - 2 ?>) \
 NBDL_MEMBER_IDS<?= ($i-1) ?>(NAME, SIZE, __VA_ARGS__)
<?php endfor; ?>
