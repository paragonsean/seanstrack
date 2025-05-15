latch_w_enbl_inst : latch_w_enbl PORT MAP (
		clock	 => clock_sig,
		enable	 => enable_sig,
		sclr	 => sclr_sig,
		data	 => data_sig,
		q	 => q_sig
	);
