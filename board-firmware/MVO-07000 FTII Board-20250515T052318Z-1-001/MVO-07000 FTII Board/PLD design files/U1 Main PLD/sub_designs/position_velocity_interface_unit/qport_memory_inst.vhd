qport_memory_inst : qport_memory PORT MAP (
		rdaddress_a	 => rdaddress_a_sig,
		data_a	 => data_a_sig,
		wraddress_a	 => wraddress_a_sig,
		wren_a	 => wren_a_sig,
		data_b	 => data_b_sig,
		wraddress_b	 => wraddress_b_sig,
		wren_b	 => wren_b_sig,
		rdaddress_b	 => rdaddress_b_sig,
		clock	 => clock_sig,
		q_a	 => q_a_sig,
		q_b	 => q_b_sig
	);
