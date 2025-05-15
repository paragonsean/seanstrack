ls_cam_inst : ls_cam PORT MAP (
		pattern	 => pattern_sig,
		wraddress	 => wraddress_sig,
		wren	 => wren_sig,
		inclock	 => inclock_sig,
		mbits	 => mbits_sig
	);
