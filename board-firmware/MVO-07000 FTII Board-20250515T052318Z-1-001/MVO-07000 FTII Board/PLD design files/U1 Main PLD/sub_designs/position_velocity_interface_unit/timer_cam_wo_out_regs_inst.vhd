timer_cam_wo_out_regs_inst : timer_cam_wo_out_regs PORT MAP (
		pattern	 => pattern_sig,
		wraddress	 => wraddress_sig,
		wren	 => wren_sig,
		inclock	 => inclock_sig,
		mbits	 => mbits_sig
	);
