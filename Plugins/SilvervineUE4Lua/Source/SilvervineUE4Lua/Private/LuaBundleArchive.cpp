// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaBundleArchive.h"

#include "LuaLog.h"

#include "Launch/Resources/Version.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


//=============================================================================================================================
// ZipStructure
//=============================================================================================================================
namespace ZipStructure
{
	enum ECompressionMethod
	{
		None = 0,
		Deflate = 8
	};

	struct LocalFileHeaderBase
	{
		enum : SIZE_T
		{
			SizeInBytes = 30
		};

		uint32 Signature;
		uint16 Version;				// Version needed to extract (minimum)
		uint16 Flag;				// General purpose bit flag
		uint16 Method;				// Compression method
		uint16 Time;				// File last modification time
		uint16 Date;				// File last modification date
		uint32 Crc;					// CRC-32
		uint32 CompressedSize;		// Compressed size
		uint32 UncompressedSize;	// Uncompressed size
		uint16 FilenameLen;			// File name length (n)
		uint16 ExtraLen;			// Extra field length (m)
	};
	struct LocalFileHeader : LocalFileHeaderBase
	{
		enum : uint32
		{
			SignatureConstant = 0x04034b50
		};
		bool Deserialize(FMemoryReader& Reader);
	};
	struct CentralDirFileHeaderBase
	{
		enum : SIZE_T
		{
			SizeInBytes = 46
		};

		uint32 Signature;
		uint16 VersionMadeby;				// Version made by
		uint16 VersionNeeded;				// Version needed to extract (minimum)
		uint16 Flag;						// General purpose bit flag
		uint16 Method;						// Compression method
		uint16 Time;						// File last modification time
		uint16 Date;						// File last modification date
		uint32 Crc;							// CRC-32
		uint32 CompressedSize;				// Compressed size
		uint32 UncompressedSize;			// Uncompressed size
		uint16 FilenameLen;					// File name length (n)
		uint16 ExtraLen;					// Extra field length (m)
		uint16 CommentLen;					// File comment length (k)
		uint16 DiskID;						// Disk number where file starts
		uint16 InternalAttributes;			// Internal file attributes
		uint32 ExternalAttributes;			// External file attributes
		uint32 OffsetLocalFileHeader = -1;	// Relative offset of local file header.
	};
	struct CentralDirFileHeader : CentralDirFileHeaderBase
	{
		enum : uint32
		{
			SignatureConstant = 0x02014b50
		};
		bool Deserialize(FMemoryReader& Reader);
	};
	struct EndOfCentralDirBlockBase
	{
		enum : SIZE_T
		{
			SizeInBytes = 22
		};
		uint32 Signature;
		uint16 DiskID;
		uint16 DiskStartOfCentralDir;
		uint16 NumOfCentralDirOnDisk;
		uint16 TotalNumOfCentralDir;
		uint32 SizeOfCentralDir;				// Size of central directory (bytes)
		uint32 OffsetStartOfCentralDir = -1;	// Offset of start of central directory
		uint16 CommentLen;						// ZIP file comment length (n)
	};
	struct EndOfCentralDirBlock : EndOfCentralDirBlockBase
	{
		enum : uint32
		{
			SignatureConstant = 0x06054b50
		};
		bool Deserialize(FMemoryReader& Reader);
	};

	// Zip Block Deserilizer
	bool LocalFileHeader::Deserialize(FMemoryReader& Reader)
	{
		if (sizeof(LocalFileHeader) == LocalFileHeader::SizeInBytes)
		{
			Reader.Serialize(this, LocalFileHeader::SizeInBytes);
		}
		else
		{
			Reader.Serialize(&Signature, sizeof(uint32));
			Reader.Serialize(&Version, sizeof(uint16));
			Reader.Serialize(&Flag, sizeof(uint16));
			Reader.Serialize(&Method, sizeof(uint16));
			Reader.Serialize(&Time, sizeof(uint16));
			Reader.Serialize(&Date, sizeof(uint16));
			Reader.Serialize(&Crc, sizeof(uint32));
			Reader.Serialize(&CompressedSize, sizeof(uint32));
			Reader.Serialize(&UncompressedSize, sizeof(uint32));
			Reader.Serialize(&FilenameLen, sizeof(uint16));
			Reader.Serialize(&ExtraLen, sizeof(uint16));
		}

		if (Signature != LocalFileHeader::SignatureConstant)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [LocalFileHeader][%s]"), Signature, __SUE4LUA_FUNCTION__);
			return false;
		}

		return true;
	}

	bool CentralDirFileHeader::Deserialize(FMemoryReader& Reader)
	{
		if (sizeof(CentralDirFileHeader) == CentralDirFileHeader::SizeInBytes)
		{
			Reader.Serialize(this, CentralDirFileHeader::SizeInBytes);
		}
		else
		{
			Reader.Serialize(&Signature, sizeof(uint32));
			Reader.Serialize(&VersionMadeby, sizeof(uint16));
			Reader.Serialize(&VersionNeeded, sizeof(uint16));
			Reader.Serialize(&Flag, sizeof(uint16));
			Reader.Serialize(&Method, sizeof(uint16));
			Reader.Serialize(&Time, sizeof(uint16));
			Reader.Serialize(&Date, sizeof(uint16));
			Reader.Serialize(&Crc, sizeof(uint32));
			Reader.Serialize(&CompressedSize, sizeof(uint32));
			Reader.Serialize(&UncompressedSize, sizeof(uint32));
			Reader.Serialize(&FilenameLen, sizeof(uint16));
			Reader.Serialize(&ExtraLen, sizeof(uint16));
			Reader.Serialize(&CommentLen, sizeof(uint16));
			Reader.Serialize(&DiskID, sizeof(uint16));
			Reader.Serialize(&InternalAttributes, sizeof(uint16));
			Reader.Serialize(&ExternalAttributes, sizeof(uint32));
			Reader.Serialize(&OffsetLocalFileHeader, sizeof(uint32));
		}

		if (Signature != CentralDirFileHeader::SignatureConstant)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [CentralDirFileHeader][%s]"), Signature, __SUE4LUA_FUNCTION__);
			return false;
		}

		return true;
	}

	bool EndOfCentralDirBlock::Deserialize(FMemoryReader& Reader)
	{
		if (sizeof(EndOfCentralDirBlock) == EndOfCentralDirBlock::SizeInBytes)
		{
			Reader.Serialize(this, EndOfCentralDirBlock::SizeInBytes);
		}
		else
		{
			Reader.Serialize(&Signature, sizeof(uint32));
			Reader.Serialize(&DiskID, sizeof(uint16));
			Reader.Serialize(&DiskStartOfCentralDir, sizeof(uint16));
			Reader.Serialize(&NumOfCentralDirOnDisk, sizeof(uint16));
			Reader.Serialize(&TotalNumOfCentralDir, sizeof(uint16));
			Reader.Serialize(&SizeOfCentralDir, sizeof(uint32));
			Reader.Serialize(&OffsetStartOfCentralDir, sizeof(uint32));
			Reader.Serialize(&CommentLen, sizeof(uint16));
		}

		if (Signature != EndOfCentralDirBlock::SignatureConstant)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [EndOfCentralDirBlock][%s]"), Signature, __SUE4LUA_FUNCTION__);
			return false;
		}

		return true;
	}
}

//=============================================================================================================================
// FSUE4LuaBundleArchive
//=============================================================================================================================
FSUE4LuaBundleArchive::FSUE4LuaBundleArchive(bool bIsPersistent, const FName BundleName)
	: FBufferArchive(bIsPersistent, BundleName)
{
}

TSharedRef<FSUE4LuaBundleArchive> FSUE4LuaBundleArchive::Create(const TCHAR* BundlePath)
{
	auto BundleArchive = TSharedRef<FSUE4LuaBundleArchive>(new FSUE4LuaBundleArchive(true, *FPaths::GetBaseFilename(BundlePath)));
	if (FFileHelper::LoadFileToArray(BundleArchive->Bytes, BundlePath))
	{
		BundleArchive->ReadCentralDirectories();
	}

	return BundleArchive;
}

bool FSUE4LuaBundleArchive::ReadFile(FString& OutResult, const TCHAR* Filename)
{
	if (Bytes.Num() == 0)
	{
		UE_LOG(LogSUE4L, Error, TEXT("There is no data to read [%s]"), __SUE4LUA_FUNCTION__);
		return false;
	}
	
	FMemoryReader Reader(Bytes, true);
	if (!LocalFileHeaderOffsetMappings.Contains(Filename))
	{
		UE_LOG(LogSUE4L, Error, TEXT("Local File Header of '%s' is not found. [%s]"), Filename, __SUE4LUA_FUNCTION__);
		return false;
	}

	uint32 LocalFileHeaderOffset = LocalFileHeaderOffsetMappings.FindRef(Filename);

	Reader.Seek(LocalFileHeaderOffset);

	// Local File header
	ZipStructure::LocalFileHeader FileHeader;
	{
		FileHeader.Deserialize(Reader);
		if (FileHeader.Signature != ZipStructure::LocalFileHeader::SignatureConstant)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [LocalFileHeader][%s]"), FileHeader.Signature, __SUE4LUA_FUNCTION__);
			return false;
		}

		Reader.Seek(Reader.Tell() + FileHeader.FilenameLen + FileHeader.ExtraLen);

		switch (FileHeader.Method)
		{
		case ZipStructure::ECompressionMethod::Deflate:
		{
			TArray<uint8> Compressed;
			Compressed.SetNumUninitialized(FileHeader.CompressedSize);
			Reader.Serialize(Compressed.GetData(), FileHeader.CompressedSize);

			TArray<uint8> Uncompressed;
			Uncompressed.SetNumUninitialized(FileHeader.UncompressedSize);

			// raw deflate data일 경우 Bitwindow를 음수로 설정해야 합니다.
			// 참조 : https://www.zlib.net/manual.html
#if 22 <= ENGINE_MINOR_VERSION
			FCompression::UncompressMemory(NAME_Zlib, Uncompressed.GetData(), FileHeader.UncompressedSize, Compressed.GetData(), FileHeader.CompressedSize, COMPRESS_NoFlags, -DEFAULT_ZLIB_BIT_WINDOW);
#else
			FCompression::UncompressMemory(ECompressionFlags::COMPRESS_ZLIB, Uncompressed.GetData(), FileHeader.UncompressedSize, Compressed.GetData(), FileHeader.CompressedSize, false, -DEFAULT_ZLIB_BIT_WINDOW);
#endif
			FFileHelper::BufferToString(OutResult, Uncompressed.GetData(), FileHeader.UncompressedSize);
			break;
		}
		case ZipStructure::ECompressionMethod::None:
			FFileHelper::BufferToString(OutResult, Bytes.GetData() + Reader.Tell(), FileHeader.UncompressedSize);
			break;
		default:
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Compression method : %d"), FileHeader.Method, __SUE4LUA_FUNCTION__);
			return false;
			break;		
		}
	}
	return true;
}

bool FSUE4LuaBundleArchive::ReadCentralDirectories()
{
	FMemoryReader Reader(this->Bytes, true);

	// 1. End of central directory record 
	// Central directory의 OffsetStartOfCentralDir와 TotalNumOfCentralDir을 찾습니다.
	// #TODO: file comment가 있는 경우에 대한 처리 (지금은 없다고 전제)

	ZipStructure::EndOfCentralDirBlock EndBlock;
	{
		Reader.Seek(Reader.TotalSize() - ZipStructure::EndOfCentralDirBlock::SizeInBytes);
		EndBlock.Deserialize(Reader);

		if (EndBlock.Signature != ZipStructure::EndOfCentralDirBlock::SignatureConstant)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [EndOfCentralDirBlock][%s]"), EndBlock.Signature, __SUE4LUA_FUNCTION__);
			return false;
		}
	}

	// 2. Central directory file header
	// 각 Central directory를 돌면서 Filename과 Local Header Offset을 매핑합니다.

	Reader.Seek(EndBlock.OffsetStartOfCentralDir);
	ZipStructure::CentralDirFileHeader CentralDir;
	{
		for (int32 Count = 1; Count <= EndBlock.TotalNumOfCentralDir; ++Count)
		{
			CentralDir.Deserialize(Reader);

			if (CentralDir.Signature != ZipStructure::CentralDirFileHeader::SignatureConstant)
			{
				UE_LOG(LogSUE4L, Error, TEXT("Invalid Signature (0x%08X) [CentralDirFileHeader][%s]"), CentralDir.Signature, __SUE4LUA_FUNCTION__);
				return false;
			}

			FString Filename;
			FFileHelper::BufferToString(Filename, Bytes.GetData() + Reader.Tell(), CentralDir.FilenameLen);

			FPaths::NormalizeFilename(Filename);

			if (!Filename.IsEmpty() && CentralDir.CompressedSize > 0)
			{
				LocalFileHeaderOffsetMappings.Add(Filename, CentralDir.OffsetLocalFileHeader);
			}

			Reader.Seek(Reader.Tell() + CentralDir.FilenameLen + CentralDir.ExtraLen + CentralDir.CommentLen);
		}
	}
	return true;
}