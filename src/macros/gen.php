<?php for ($i = 63; $i > 3; $i--): ?>
#define NBDL_MEMBER_NAMES<?= $i ?>(NAME, MNAME, ...) NBDL_MEMBER_NAMES2(NAME, MNAME), 
 NBDL_MEMBER_NAMES<?= ($i-1) ?>(NAME, __VA_ARGS__)
<?php endfor; ?>
