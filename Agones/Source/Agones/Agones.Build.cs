// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using System.Text.RegularExpressions;

namespace UnrealBuildTool.Rules
{
	public class Agones : ModuleRules
	{
		public Agones(ReadOnlyTargetRules Target) : base(Target)
		{
		    PrivateIncludePaths.AddRange(
            new string[] {
            "Agones/Private",
            }
            );

            PublicDefinitions.Add("GPR_FORBID_UNREACHABLE_CODE=1");
            PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
            PublicDependencyModuleNames.AddRange(
            new string[]
            {
            "Core",
            "Projects",
			"OpenSSL",
			"zlib",
            "GRPC"
            }
            );
            PublicIncludePathModuleNames.AddRange(
				new string[]
                {
                    "GRPC"
                }
                );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );
			
		

            if (Target.Type == TargetRules.TargetType.Server)
            {
                if (Target.Platform == UnrealTargetPlatform.Linux)
                {
                    PublicDefinitions.Add("ENABLE_AGONES=1");

                    string BaseDirectory = System.IO.Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, ".."));
                    string SDKDirectory = System.IO.Path.Combine(BaseDirectory, "ThirdParty", "x86_64-unknown-linux-gnu");
                    //PublicIncludePaths.Add(System.IO.Path.Combine(SDKDirectory, "include"));

                    //    PublicLibraryPaths.Add(SDKDirectory);
                    //    PublicAdditionalLibraries.Add(System.IO.Path.Combine(SDKDirectory, "libagonessdk.a"));
                    PublicAdditionalLibraries.Add(System.IO.Path.Combine(SDKDirectory, "libgpr.a"));
                    PublicAdditionalLibraries.Add(System.IO.Path.Combine(SDKDirectory, "libgrpc.a"));
                    PublicAdditionalLibraries.Add(System.IO.Path.Combine(SDKDirectory, "libgrpc_unsecure.a"));
                    PublicAdditionalLibraries.Add(System.IO.Path.Combine(SDKDirectory, "libprotobuf.a"));
                }
                else
                {
                    PublicDefinitions.Add("ENABLE_AGONES=0");
                }
            }
            else
            {
                PublicDefinitions.Add("ENABLE_AGONES=0");
            }
		}
	}
}
