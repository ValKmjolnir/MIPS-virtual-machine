add 	$s0, $zero, $zero		# $s0 = $zero + $zero set A
addi	$s1, $zero, 40			# $s1 = $zero + 40 set B
addi 	$s2, $zero, 80			# $s2 = $zero + 80 set C
assign_B:
add     $t0, $s1,   $zero       # $t0 = $s1 get B
addi    $t1, $zero, 0xaa        # B[0~9] = 0xaa
addi    $t2, $s1,   40          # end loop condition
loop_B:
sw		$t1, 0($t0)             # B[i] = t1
addi    $t0, $t0,   4           # $t0 = $t0 + 4
beq     $t0, $t2,   assign_C    # $t0 = $t2 end loop
j       loop_B
assign_C:
add     $t0, $s2,   $zero       # $t0 = $s1 get C
addi    $t1, $zero, 0xff        # C[0~9] = 0xff
addi    $t2, $s2,   40          # end loop condition
loop_C:
sw		$t1, 0($t0)             # C[i] = t1
addi    $t0, $t0,   4           # $t0 = $t0 + 4
beq     $t0, $t2,   assign_A    # $t0 = $t2 end loop
j       loop_C
assign_A:
add     $t0, $s0,   $zero       # $t0 = $s0 get A
add     $t1, $s1,   $zero       # $t1 = $s1 get B
add     $t2, $s2,   $zero       # $t2 = $s2 get C
addi    $t3, $s0,   40          # end loop condition
loop_A:
lw      $t4, 0($t1)             # get B[i]
lw      $t5, 0($t2)             # get C[i]
add     $t6, $t4,   $t5         # $t6 = B[i] + C[i]
sw      $t6, 0($t0)             # A[i] = $t6
addi    $t0, $t0,   4           # $t0 + 4
addi    $t1, $t1,   4           # $t1 + 4
addi    $t2, $t2,   4           # $t2 + 4
beq     $t0, $t3    exit        # $t0 = $t3 end loop
j       loop_A
exit:

# machine code
# 0x00008020
# 0x20110028
# 0x20120050
# 0x02204020
# 0x200900aa
# 0x222a0028
# 0xad090000
# 0x21080004
# 0x11480001
# 0x08000006
# 0x02404020
# 0x200900ff
# 0x224a0028
# 0xad090000
# 0x21080004
# 0x11480001
# 0x0800000d
# 0x02004020
# 0x02204820
# 0x02405020
# 0x220b0028
# 0x8d2c0000
# 0x8d4d0000
# 0x018d7020
# 0xad0e0000
# 0x21080004
# 0x21290004
# 0x214a0004
# 0x11680001
# 0x08000015
