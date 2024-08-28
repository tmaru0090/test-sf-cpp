function initialize()
	path =ZDialog2(Dialog.FileSelection,"初期化ファイルを選択してくださーい")
	print(path)
end
function finalize()
	print("終了ー")
end
cnt = 0
function mainloop()
	getJPressCnt(0,CustomKey.Cross,cnt)
	print("カウント: ",cnt)
	if cnt == 1 then
		print("☓ボタン押された")
	end
end
